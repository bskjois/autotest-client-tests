/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
 *
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or 
 *   (at your option) any later version.
 *   This program is distributed in the hope that it will be useful, 
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *   GNU General Public License for more details. 
 *   You should have received a copy of the GNU General Public License 
 *   along with this program; if not, write to the Free Software 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *   USA 
 *
 * Author(s):
 *      Carl McAdams <carlmc@us.ibm.com>
 *      Donald A. Barre <dbarre@unh.edu>
 *
 * Spec:        HPI-B.01.01
 * Function:    saHpiIdrFieldSet
 * Description:
 *   Pass in an Invalid Text Buffer in the Field Structure. Set the Data 
 *   Type to an out-of-range value.
 *   Expected return: SA_ERR_HPI_INVALID_PARAMS.
 * Line:        P111-35:P112-1
 *    
 */

#include <stdio.h>
#include "../include/inventory_test.h"

/*************************************************************************
 *
 * Test setting an invalid DataType.  
 *
 *************************************************************************/

int TestField(SaHpiSessionIdT sessionId,
	      SaHpiResourceIdT resourceId,
	      SaHpiIdrIdT IdrId, SaHpiEntryIdT AreaId)
{
	SaErrorT status;
	int retval;
	SaHpiIdrFieldT Field;

	status = addCustomField(sessionId, resourceId, IdrId, AreaId, &Field);
	if (status == SA_ERR_HPI_INVALID_DATA) {
		retval = SAF_TEST_NOTSUPPORT;
	} else if (status == SA_ERR_HPI_OUT_OF_SPACE) {
		retval = SAF_TEST_NOTSUPPORT;
	} else if (status != SA_OK) {
		retval = SAF_TEST_UNRESOLVED;
		e_print(saHpiIdrFieldAdd, SA_OK, status);
	} else {

		Field.Field.DataType = SAHPI_TL_TYPE_BINARY + 1;
		status = saHpiIdrFieldSet(sessionId, resourceId, IdrId, &Field);

		if (status == SA_ERR_HPI_INVALID_PARAMS) {
			retval = SAF_TEST_PASS;
		} else {
			retval = SAF_TEST_FAIL;
			e_print(saHpiIdrFieldSet,
				SA_ERR_HPI_INVALID_PARAMS, status);
		}

		// Clean up
		status = saHpiIdrFieldDelete(sessionId, resourceId,
					     IdrId, AreaId, Field.FieldId);
		if (status != SA_OK) {
			e_print(saHpiIdrFieldDelete, SA_OK, status);
		}
	}

	return retval;
}

/*************************************************************************
 *
 * Traverse the Areas in a IDR.  We will stop the traversing when
 * we find a writable Area that we can test.
 *
 *************************************************************************/

int processInventoryRdr(SaHpiSessionIdT sessionId,
			SaHpiResourceIdT resourceId,
			SaHpiRdrT * rdr, SaHpiInventoryRecT * inventoryRec)
{
	SaErrorT status;
	int retval;
	SaHpiEntryIdT AreaId, NextAreaId;
	SaHpiIdrAreaHeaderT Header;
	SaHpiIdrInfoT Info;

	status = saHpiIdrInfoGet(sessionId, resourceId,
				 inventoryRec->IdrId, &Info);

	if (status != SA_OK) {
		retval = SAF_TEST_UNRESOLVED;
		e_print(saHpiIdrInfoGet, SA_OK, status);
	} else if (Info.ReadOnly) {
		retval = SAF_TEST_NOTSUPPORT;
	} else {

		retval = SAF_TEST_NOTSUPPORT;

		NextAreaId = SAHPI_FIRST_ENTRY;
		while ((retval == SAF_TEST_NOTSUPPORT) &&
		       (NextAreaId != SAHPI_LAST_ENTRY)) {

			AreaId = NextAreaId;
			status = saHpiIdrAreaHeaderGet(sessionId,
						       resourceId,
						       inventoryRec->IdrId,
						       SAHPI_IDR_AREATYPE_UNSPECIFIED,
						       AreaId,
						       &NextAreaId, &Header);

			if (status == SA_ERR_HPI_NOT_PRESENT) {
				// do nothing
			} else if (status != SA_OK) {

				retval = SAF_TEST_UNRESOLVED;
				e_print(saHpiIdrAreaHeaderGet, SA_OK, status);

			} else if (!Header.ReadOnly) {

				retval = TestField(sessionId, resourceId,
						   inventoryRec->IdrId, AreaId);
			}
		}
	}

	return retval;
}

/*************************************************************************
 *
 *  Process all Inventory RDRs.  The below macro expands to
 *  generate all of the generic codes necessary to call the given
 *  function to process an RDR.
 *
 *************************************************************************/

processAllInventoryRdrs(processInventoryRdr)
