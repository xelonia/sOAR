/**
* \file UnitTest.cpp
* \brief Implementation of UnitTest baseclass 
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "UnitTest.h"
#include "UnitTestManager.h"

UnitTest::UnitTest(char *name) {
	UnitTestManager::GetInstance()->RegisterUnitTestClass(this, name);
}

void UnitTest::TestGroup(char *name)	{ 
	UnitTestManager::GetInstance()->TestGroup(name); 
}

bool UnitTest::ExpectFail( bool result, char *str, ... ) { 
	if (result) {
		va_list args;
		va_start(args, str);

		UnitTestManager::GetInstance()->HandleFailedTest(str, args); 

		va_end(args);
	}
	else {
		UnitTestManager::GetInstance()->HandlePassedTest();
	}
	return result; 
}

bool UnitTest::ExpectOkay( bool result, char *str, ...)  { 
	if (!result) {
		va_list args;
		va_start(args, str);

		UnitTestManager::GetInstance()->HandleFailedTest(str, args); 

		va_end(args);
	}
	else {
		UnitTestManager::GetInstance()->HandlePassedTest();
	}
	return result; 
}

char *UnitTest::GetInputPath() {
	return UnitTestManager::GetInstance()->GetInputPath();
}

double UnitTest::GetNextTestValue(int &idx, int inc) {
	return UnitTestManager::GetInstance()->GetNextTestValue(idx,inc);
}
	
int UnitTest::GetNextIntTestValue(int &idx, int minVal, int maxVal, int inc) {
	return UnitTestManager::GetInstance()->GetNextIntTestValue(idx,minVal,maxVal,inc);
}
