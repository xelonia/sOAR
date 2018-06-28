/**
* \file UtForward.cpp
* \brief Implementation of UtForward to test the Forward class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#include "UnitTest.h"

#include "../soar_lib/Decision.h"
#include "../soar_lib/Forward.h"
#include "../soar_lib/Settings.h"


/**
 * \ingroup Tests
 * \brief UnitTest for Forward class
 */
class UtForward: public UnitTest {
private:

public:

	/// \brief The constructor registers at the UnitTestManager 
	UtForward() : UnitTest("Forward")
	{
	}

	void RunTests() {		
	}
};

UtForward test_Forward;  ///< Global instance automatically registers to UnitTestManager
