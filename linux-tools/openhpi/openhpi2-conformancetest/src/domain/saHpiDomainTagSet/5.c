/*
 * (C) Copyright IBM Corp. 2004, 2005
 * Copyright (c) 2005, Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307 USA.
 *
 * Author(s):
 *      Carl McAdams <carlmc@us.ibm.com>
 *      Xiaowei Yang <xiaowei.yang@intel.com>
 *
 * Spec:        HPI-B.01.01
 * Function:    saHpiDomainTagSet
 * Description:   
 *   Set the DomainTag->DataType to SAHPI_TL_TYPE_TEXT, and
 *   the DomainTag->Language to an out-of-range value.
 *   Expected return: SA_ERR_HPI_INVALID_PARAMS.
 * Line:        P38-18:P38-19
 */
#include <stdio.h>
#include <string.h>
#include "saf_test.h"

int run_test(SaHpiSessionIdT sessionId, SaHpiTextTypeT dataType)
{
	SaHpiDomainInfoT DomainInfo;
	SaHpiTextBufferT NewTag;
	SaErrorT status;
	int retval = SAF_TEST_UNKNOWN;

	// set up the test Tag to write into the DomainTag
	NewTag.DataType = dataType;
	NewTag.Language = SAHPI_LANG_ZULU + 1;	//out-of-range
	NewTag.DataLength = 0;	// clear tag

	//
	// Retrieve the current DomainTag
	// 
	status = saHpiDomainInfoGet(sessionId, &DomainInfo);

	if (status != SA_OK) {
		e_print(saHpiDomainInfoGet, SA_OK, status);
		retval = SAF_TEST_UNRESOLVED;
	} else {
		// Set the DomainTag->DataType to SAHPI_TL_TYPE_TEXT, and
		//   the DomainTag->Language to an out-of-range value.
		status = saHpiDomainTagSet(sessionId, &NewTag);

		if (status != SA_ERR_HPI_INVALID_PARAMS) {
			e_print(saHpiDomainTagSet,
				SA_ERR_HPI_INVALID_PARAMS, status);
			retval = SAF_TEST_FAIL;
		} else
			retval = SAF_TEST_PASS;

		//restore the Domain Tag to what it previously was.
		status = saHpiDomainTagSet(sessionId, &DomainInfo.DomainTag);

		if (status != SA_OK) {
			m_print
			    ("Domain Tag was not restored to its previous value!");
			e_print(saHpiDomainTagSet, SA_OK, status);
		}
	}

	return (retval);
}

int Test_Domain(SaHpiSessionIdT sessionId)
{
	int retval;

	retval = run_test(sessionId, SAHPI_TL_TYPE_TEXT);
	if (retval == SAF_TEST_PASS) {
		retval = run_test(sessionId, SAHPI_TL_TYPE_UNICODE);
	}

	return retval;
}

/**********************************************************
 *   Main Function
 *      takes no arguments
 *
 *       returns: SAF_TEST_PASS when successfull
 *                SAF_TEST_FAIL when an unexpected error occurs
 *************************************************************/
int main(int argc, char **argv)
{
	int retval = SAF_TEST_UNKNOWN;

	retval = process_all_domains(NULL, NULL, Test_Domain);

	return (retval);
}
