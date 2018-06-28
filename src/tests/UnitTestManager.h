/**
* \file UnitTestManager.h
* \brief Declaration of UnitTestManager class for managing unit tests
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#ifndef UNITTEST_MANAGER_H
#define UNITTEST_MANAGER_H

#include <stdarg.h>


class  UnitTest;

/**
* \defgroup Tests Tests
* This module contains the UnitTest and UnitTestManager classes used for testing the sOAR_Lib and sOAR_Support_Lib.
*
* <DIV class="groupHeader">Overview</DIV>
* - All unit tests classes derive from the UnitTest base class that enforces that the
*   UnitTest.RunTests() is implemented. 
*
* - The classes are each defined in a single .cpp File and instantiates a global instance of the own class 
*   at the end of its *.cpp file. During their construction they 'self-register' at the singletone instance
*   of the UnitTestManager.
*
* - The main() function at the very end of the UnitTestManager.cpp is called after the instantiation
*   of all global variables (in this case the unit test instances). 
*
* - The main() executable function
*   then calls the UnitTestmanager.RunAllTests() that starts testing the registered classes.
*/

/**
 * \ingroup Tests
 * \brief Manager for classes derived from UnitTest.
 *
 * The UnitTestManager calls the UnitTest::RunTests() function of 
 * all registered unit test classes.
 *
 * These test classes then call either 
 * - ExpectFail()
 * - ExpectOkay()
 * of the UnitTestManager depending on the expected test result.
 *
 * The UnitTestManager keeps track of failed and passed unit tests and
 * outputs test results to the console and into a logfile also containing
 * the stdout output off all tests.
 */
class UnitTestManager {
private:
	static UnitTestManager *_singleton;			///< Pointer to singleton instance

	/**
	* \ingroup Tests
	* \brief Local struct that stores UnitTest related information, managed by UnitTestManager
	*/
	struct UnitTestData {
	private:
		friend class UnitTestManager;

		UnitTestData * _nextTest;	///< Linked list of UnitTestData, linking tests
		UnitTestData * _nextGroup;  ///< Linked list of UnitTestData, linking groups of a test

		UnitTest      * _test;		///< Pointer to the UnitTest
		char          * _name;		///< Testname or groupname

		int             _failed;	///< Number of failed tests
		int             _passed;	///< Number of passed tests

		/// \brief private Constructor
		UnitTestData(UnitTest *test, char *name) { 
			_test      = test;
			_name      = name;
			_passed    = 0;
			_failed    = 0;
			_nextTest  = 0;
			_nextGroup = 0;
		}
	};

	UnitTestData           *_head;				///< Pointer to head of list of unit tests
	UnitTestData           *_curr;				///< Pointer to current class beeing tested
	UnitTestData           *_group;				///< Pointer to current group beeing tested

	char                    _outPath[256];		///< Pathname for all output files
	char                    _inpPath[256];		///< Pathname for all input  files

	double _testValues[1000];	                ///< Array of test values filled with regular and random numbers
	int    _testNum;			                ///< Number of testValues

	UnitTestManager(); 	  ///< Constructor is private to enforce Singleton pattern

	/// Initialize an array of random test values
	void InitPseudoRandomTestValues();

	/// Retrieves the +/- range of a given testvalue
	double GetRangeOfTestValue(int idx);

	/// Output to stderr and into logfile
	void OutVA(char *str, va_list args);

	/// Output to stderr and into logfile
	void Out(char *str, ...);

	/// Print test result as banner
	void OutResultBanner(bool passed, bool big);

	/// Print total test results
	void OutputResult();

	/// Print list of tests not yet implemented
	void OutputMissingTests();

protected:
	friend class UnitTest;

	/// \brief Registers a derived UnitTest class 
	void RegisterUnitTestClass(UnitTest *unitTestClass, char *testName);

	/**
	 * \brief Stores the name of the current group of tests.
	 * If no parameter is supplied, then the group is ended.
	 *
	 * \param name  Name of group or 0 if group ends
	 */
	void TestGroup(char *name=0);

	/**
	 * \brief Callback called by classes derived from UnitTest for failed tests
     * Ouputs failed message and updates statistics
	 * \param str       Printf format string with information about the test
	 * \param args      Variable argument lis also forwarded to printf
	 */
	void HandleFailedTest(char *str, va_list args);

	/**
	 * \brief Callback called by classes derived from UnitTest for passed tests
     * Updates statistics
	 */
	void HandlePassedTest();

	/// \brief Retrieves path for UnitTest input files
	char *GetInputPath();

	/// \brief Retrieves a testvalue and increments/wraps index
	double GetNextTestValue(int &idx, int inc=1);

	/// \brief Retrieves a integer testvalue in the given range and increments/wraps index
	int GetNextIntTestValue(int &idx, int minVal, int maxVal, int inc=1);

public:
	/**
	* \brief Singleton instance retrieval.
	* Using the sington programmng pattern garantees that there is only one 
	* instance that is accessible to all other classes.
	* \return Pointer to single instance
	*/
	static UnitTestManager *GetInstance()	{
		if (_singleton == 0) {
			_singleton = new UnitTestManager();
		}
		return _singleton;
	}

	/// \brief Function to be called from main to execute all tests of all registered classes.
	void RunAllTests();
};

#endif // UNITTEST_MANAGER_H