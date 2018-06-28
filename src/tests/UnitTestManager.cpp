/**
* \file UnitTestManager.cpp
* \brief Implementation of UnitTestManager class for managing unit tests
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>		// For _mkdir()

#include "UnitTestManager.h"
#include "UnitTest.h"

UnitTestManager * UnitTestManager::_singleton = 0;

UnitTestManager::UnitTestManager() {
	_head            = 0;
	_group           = 0;
	_curr            = 0;

	// Generate the array of pseudo random numbers used as test values
	InitPseudoRandomTestValues();

	strcpy_s(_outPath,"./tests_tmp");
	strcpy_s(_inpPath,"../tests");		// Relative to output dir, which is current path !

	// Create output directory
	_mkdir(_outPath);
	_chdir(_outPath);

	// Prepare logfile name with date and time
	char logfileName[100];
	time_t now   = time(NULL);
	struct tm t;		
	localtime_s(&t, &now);

	sprintf_s(logfileName, "UnitTest_Log_%04d_%02d_%02d__%02d_%02d.txt", 
		t.tm_year + 1900, t.tm_mon+1, t.tm_mday, t.tm_hour,t.tm_min);


	FILE *fp;
	freopen_s(&fp, logfileName, "w", stdout );

	Out("_  _ _  _ _ ___ ___ ____ ____ ___    _  _ ____ _  _ ____ ____ ____ ____  \n");
	Out("|  | |\\ | |  |   |  |___ [__   |     |\\/| |__| |\\ | |__| | __ |___ |__/  \n");
	Out("|__| | \\| |  |   |  |___ ___]  |     |  | |  | | \\| |  | |__] |___ |  \\  \n");
	Out("------------------------------------------------------------------------\n");
	Out("  Path: %-15s    LogFile: %s\n\n",_outPath, logfileName);
}


void UnitTestManager::RegisterUnitTestClass(UnitTest *unitTestClass, char *testName) {
	UnitTestData * uts = new UnitTestData(unitTestClass,testName);

	if (_head) {
		uts->_nextTest = _head;
	}
	_head = uts;
}


char *UnitTestManager::GetInputPath() {
	return _inpPath;
}

void UnitTestManager::OutVA(char *str, va_list args)
{
	vfprintf(stderr, str, args);
	vfprintf(stdout, str, args);
	fflush(stdout);
}

void UnitTestManager::Out(char *str, ...)
{
	va_list args;
	va_start(args, str);
	OutVA(str, args);
    va_end(args);
}

void UnitTestManager::RunAllTests() {
	_curr = _head;

	while(_curr) {
		_curr->_failed = 0;
		_curr->_passed = 0;

		Out("\nTesting %s\n",_curr->_name);
		Out("========================================================\n");

		_curr->_test->RunTests();
		TestGroup(); // Close group, if anything was open

		Out("-------------------------------------------------\n");

		if (_curr->_failed == 0 && _curr->_passed==0) {
			Out("%-20s                  --- ------\n",_curr->_name);
		}
		else if (_curr->_failed == 0 && _curr->_passed>0) {
			Out("%-20s              %7d Tests\n",_curr->_name,_curr->_passed);
		}
		else {
			Out("%-20s%6d Tests ==>%4d  FAILED\n",_curr->_name,_curr->_failed+_curr->_passed, _curr->_failed);
		}
		OutResultBanner(_curr->_failed == 0, false);

		_curr = _curr->_nextTest;
	}

	OutputMissingTests();
	OutputResult();
}

void UnitTestManager::OutputMissingTests() {

	bool missingTests = false; 
	_curr = _head;
	while(_curr && !missingTests) {
		if (_curr->_failed == 0 && _curr->_passed>0) {
			missingTests = true;
		}
		else {
			// Check groups for missing tests
			UnitTestData * grp = _curr->_nextGroup;
			while(grp) {
				if (grp->_failed==0 && grp->_passed==0) {
					missingTests = true;
					break;
				}
				grp = grp->_nextGroup;
			}
		}
		_curr = _curr->_nextTest;
	}

	if (!missingTests)
		return;

	Out("\n");
	Out("========================================================================\n");
	Out("Tests not yet implemented\n\n");

	_curr = _head;
	while(_curr) {
		if (_curr->_failed == 0 && _curr->_passed==0) {
			Out("NYI  | %18s | -- no tests at all --\n",_curr->_name);
		}
		else {
			UnitTestData * grp = _curr->_nextGroup;
			while(grp) {
				if (grp->_failed==0 && grp->_passed==0) {
					Out("NYI  | %18s | %-25s\n",_curr->_name, grp->_name);
				}
				grp = grp->_nextGroup;
			}

		}
		_curr = _curr->_nextTest;
	}
}

void UnitTestManager::OutputResult() {
	int totalPassed = 0, totalFailed = 0;

	Out("\n");
	Out("========================================================================\n");
	Out("Modules\n");

	_curr = _head;
	while(_curr) {
		totalPassed += _curr->_passed;
		totalFailed += _curr->_failed;
		
		if (_curr->_failed == 0 && _curr->_passed>0) {
			Out("       %6d PASSED  %-25s\n",_curr->_passed,_curr->_name);
		}
		else if (_curr->_failed > 0) {
			Out("       %6d Tests   %-25s=> FAILED%5d\n",
				_curr->_failed+_curr->_passed,_curr->_name, _curr->_failed);
		}

		_curr = _curr->_nextTest;
	}


	Out("\n");
	if (totalFailed==0)
		Out("TOTAL%8d\n",totalPassed);
	else
		Out("TOTAL%8d                               ====> FAILED%5d\n",
		     totalPassed+totalFailed, totalFailed);
	Out("--------------------------------------------------------------\n");

	OutResultBanner(totalFailed==0, true);	
}

void UnitTestManager::TestGroup(char *name) {

	if (_group) {
		// Close the current group
		if (_group->_passed==0 && _group->_failed==0) {
			// Empty group
			fprintf(stdout,"| %-30s |",_group->_name);
			Out("    --- ------\n",_group->_name);
		}
		else if (_group->_passed>0 && _group->_failed==0) {
			// Passed group
			fprintf(stdout,"| %-30s |",_group->_name);
			Out(" %6d PASSED\n",_group->_passed);
		}

		_group = 0;
	}

	if (name) {
		_group = new UnitTestData(0, name);

		if ( _curr->_nextGroup ) {
			_group->_nextGroup = _curr->_nextGroup;
		}
		_curr->_nextGroup = _group;		
		Out("| %-30s |",_group->_name);
		fprintf(stdout,"\n---------------------------------------\n");
	}
}

void UnitTestManager::HandleFailedTest(char *str, va_list args) {
	_curr->_failed++;
	
	if (_group) {
		if (_group->_failed>0) {
			Out("| %-30s |",_group->_name);
		}
		fprintf(stdout,"| %-30s |",_group->_name);
		Out("   ==>  FAILED ");
		_group->_failed++;
	}
	else {
		Out("                                 |   ==>  FAILED ");
	}
	OutVA(str, args);
	Out("\n");
}

void UnitTestManager::HandlePassedTest() {
	_curr->_passed++;
	if (_group) {
		_group->_passed++;
	}
}

void UnitTestManager::OutResultBanner(bool passed, bool big)
{
	if (big) {
		if (passed) {
			Out("  ########     ###     ######   ######  ######## ########  \n");
			Out("  ##     ##   ## ##   ##    ## ##    ## ##       ##     ## \n"); 
			Out("  ##     ##  ##   ##  ##       ##       ##       ##     ## \n"); 
			Out("  ########  ##     ##  ######   ######  ######   ##     ## \n"); 
			Out("  ##        #########       ##       ## ##       ##     ## \n"); 
			Out("  ##        ##     ## ##    ## ##    ## ##       ##     ## \n"); 
			Out("  ##        ##     ##  ######   ######  ######## ########  \n");
		}
		else {
			Out("  ......................................................... \n");
			Out(" '########::::'###::::'####:'##:::::::'########:'########:: \n");
			Out("  ##.....::::'## ##:::. ##:: ##::::::: ##.....:: ##.... ##: \n");
			Out("  ##::::::::'##:. ##::: ##:: ##::::::: ##::::::: ##:::: ##: \n");
			Out("  ######:::'##:::. ##:: ##:: ##::::::: ######::: ##:::: ##: \n");
			Out("  ##...:::: #########:: ##:: ##::::::: ##...:::: ##:::: ##: \n");
			Out("  ##::::::: ##.... ##:: ##:: ##::::::: ##::::::: ##:::: ##: \n");
			Out("  ##::::::: ##:::: ##:'####: ########: ########: ########:: \n");
			Out(" ..::::::::..:::::..::....::........::........::........::: \n");
		}
	}
	else {
		if (passed) {
			Out("     ___  _   __  __  ___  __   \n");
			Out("    | o \\/ \\ / _|/ _|| __||  \\  \n");
			Out("    |  _/ o |\\_ \\\\_ \\| _| | o ) \n");
			Out("    |_| |_n_||__/|__/|___||__/  \n");
		}
		else {
			Out("     ___  _   _  _    ___  __   \n");
			Out("    | __|/ \\ | || |  | __||  \\  \n");
			Out("    | _|| o || || |_ | _| | o ) \n");
			Out("    |_| |_n_||_||___||___||__/  \n");
		}
		Out("\n");
		Out("\n");
	}
}

void   UnitTestManager::InitPseudoRandomTestValues() {

	// Always use the same random numbers
	srand(0);

	_testNum = sizeof(_testValues)/sizeof(double);

	int i=0;
	// First 100 regular numbers -25 .. +25 in 0.5 increments
	while(i<100 && i<_testNum) {
		_testValues[i++] = (i-50) * 0.5;
	}

	// Numbers in different ranges  +/- 5, +/-50, +/-500, +/-50000, +/- 6127283
	while(i<_testNum) {
		double r0_1 = (double)rand() / (double)RAND_MAX;
		_testValues[i++] = (r0_1 - 0.5) * GetRangeOfTestValue(i);
	}
}


double UnitTestManager::GetRangeOfTestValue(int idx) {
	if (idx<100)
		return 50;
	else if (idx<200)
		return 10;
	else if (idx<400)
		return 100;
	else if (idx<500)
		return 1000;
	else if (idx<600)
		return 100000;	
	return  1234567.890123;
}

double UnitTestManager::GetNextTestValue(int &idx, int inc) {
	double ret = _testValues[idx];
	idx += inc;
	if (idx >= _testNum)
		idx -= _testNum;
	return ret;
}


int UnitTestManager::GetNextIntTestValue(int &idx, int minVal, int maxVal, int inc) {
	double r   = GetRangeOfTestValue(idx);
	double val = GetNextTestValue(idx,inc);
	
	// Rescale value to range -0.5 .. +05
	double rangedVal = val/r;	

	double newVal    = minVal + (rangedVal+0.5) * (maxVal-minVal);

	return (int)newVal;
}

/****
 * The main executable entry for UnitTests
 */
int main(int argc, const char * argv[]) {
	UnitTestManager::GetInstance()->RunAllTests();

	getchar();
	return 0;
}
