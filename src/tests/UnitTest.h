/**
* \file UnitTest.h
* \brief Declaration of partly abstract baseclass for UnitTests 
*     
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#ifndef UNITTEST_H
#define UNITTEST_H

#define UNIT_TEST	///< This define is used to toggle  assert() into an exception 

/**
 * \ingroup Tests
 * \brief Abstract baseclass for UnitTests
 *
 * A derived class will register itself with the UnitTestManager. Later its
 * RunTests() function will be called and in each test the derived class 
 * should call either 
 * - ExpectFail()
 * - ExpectOkay()
 * depending on the expected test result.
 */
class UnitTest {
public:

	/// \brief The constructor registers the class at the UnitTestManager
	UnitTest(char *name);

	/// \brief All derived classes need to define a RunTests(() function
	virtual void RunTests() = 0;
	
	/**
	 * \brief Starts/ends a group of tests. 
	 * When no parameter is supplied (or 0) then the group is ended.
	 * If te group changes, then the previous group information success 
	 * state is printed
	 * \param name      The group info string or 0
	 */
	void TestGroup(char *name=0);

	/**
	 * \brief This function should be called, if result is expected to be false.
	 * \param result    The test result that is expected to be false
	 * \param str       Char *  and variable parameters are forwarded to printf
	 * \returns         The test result
	 */
	bool ExpectFail( bool result, char *str, ... );

	/**
	 * \brief This function should be called, if result is expected to be true.
	 * \param result    The test result that is expected to be true
	 * \param str       Char *  and variable parameters are forwarded to printf
	 * \returns         The test result
	 */
	bool ExpectOkay( bool result, char *str, ...);

	
	/// \brief Retrieves path for UnitTest input files (without trailing /)
	char *GetInputPath();

	/**
	 * \brief Retrieves a testvalue from a pool and increments/wraps the index.
	 * \param idx  Index, managed by the UnitTestManager
	 * \param inc  Optional parameter to specify index increment
	 */
	double GetNextTestValue(int &idx, int inc=1);
	
	/**
	 * \brief Retrieves a integer testvalue in a given range from the pool 
	 * \param idx    Index, managed by the UnitTestManager
	 * \param minVal Minimal allowed value
	 * \param maxVal Maximum allowed value
	 * \param inc    Optional parameter to specify index increment
	 */
	int GetNextIntTestValue(int &idx, int minVal, int maxVal, int inc=1);
};

#endif //UNITTEST_H