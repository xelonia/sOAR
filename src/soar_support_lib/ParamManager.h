/**
* \file ParamManager.h
* \brief Declaration of class ParamManager
*
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
*/

#ifndef PARAM_MANAGER_H
#define PARAM_MANAGER_H

#include <Math.h>
#include <stdio.h>
#include <stdlib.h>

#include "FuncType.h"
#include "NArray.h"

/**
* \defgroup SoarSupport sOAR_Support
* This module contains classes that support the sOAR library but are not directly related to the sOAR problem domain.
* - NArray : Fast N dimensional array access
* - FuncType : Variable function type class
* - ParamManager : A parameter manager that allows saving, loading and comparison of class 
*   parameters in binary/ascii configuration/result files. <BR>
*   The manager itself uses the LibconfigWrapper class to extend soome features of
*   the external libconfig library ( http://www.hyperrealm.com/libconfig/ )
* - NanoTimer : A timer class with nano second resolution
*
* <BR/>
* <DIV class="groupHeader">Copyright</DIV>
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
* <HR />
*/

class LibconfigWrapper;

/**
* \ingroup SoarSupport
* \ingroup ParamManager
* \ingroup SoarLib
* \brief Manages parameter serialization and deserialization for ascii and binary files.
*
*
* <div class="groupHeader">Versions</div>
* \code 
*   31.07.2017 Refactored ParBase, ParPrim, ParArray,ParClass and ParGroup as private ParamManager classes 
*   02.04.2016 Condensed templates, reduced number of functions
*   18.03.2016 Added support for short and ushort as base types, array types and NArray types
*   16.03.2016 Added support for char and uchar as base types, array types and NArray types
*   15.03.2016 Added support for arrays of arbitary size
*   06.03.2016 Added support for comparison of ascii file
*   06.03.2016 Added support for strings
*   05.03.2016 Added support for grouping
*   03.03.2016 Added LibconfigWrapper for saving
*   01.03.2016 Added LibconfigWrapper for loading
*   29.02.2016 Restructured
*   17.01.2016 Initial version
* \endcode
* 
*
* <div class="groupHeader">Copyright</div>
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
* <HR />
*
* <H2 class="groupheader">Include and Collaboration Diagram</H2>
*/
class ParamManager {
	
	// Forward declaration of private support classes
	class ParBase;
	class ParPrim;
	class ParArray;
	template<class T> class ParClass;
	class ParGroup;

#ifdef UNIT_TEST
	friend class UtParamManager;		///< Allow access to private classes for unit tests
	friend class UtLibconfigWrapper;	///< Allow access to private classes for unit tests
#endif

private:
	ParBase *_head;			///< Head of linked list of parameters
	ParBase *_last;			///< Pointer to last element of linked list of parameters
	ParBase *_curr;			///< Pointer to currently loaded/saved,comapred parameter

	/// \brief Adds a new parameter to the end of the linked list
	void Link(ParBase *pb);	


#ifdef GENERATE_DOXYGEN_DOC
	ParPrim         * doxygen;  ///< DOXYGEN: Each added primitive variable is stored in _head list
	ParClass        * doxygen;  ///< DOXYGEN: Each added class variable is stored in _head list
	ParGroup        * doxygen;  ///< DOXYGEN: Each added group Beg and GroupEnd a pseudo parameter is stored in _head list
	ParArray        * doxygen;  ///< DOXYGEN: Each added const arry variable is stored in _head list
	LibconfigWrapper  doxygen;  ///< DOXYGEN: LoadAscii and SaveAscii use a wrapper instance
#endif	

	/// \brief Template member to add constant and dynamic array parameters
	template<typename T> void AddArray(const T * addr, char *cfg_name, bool cfg_optional, unsigned int N);

	/**
	 * \name Private member functions used for the compare functionality 
	 * @{ 
	 */
	
	template<typename T> bool CompareAsciiBuildin(  LibconfigWrapper *lcw, ParBase *curr);
	template<typename T> bool CompareAsciiArray(    LibconfigWrapper *lcw, ParBase *curr);
	template<typename T> bool CompareAsciiNArray(   LibconfigWrapper *lcw, ParBase *curr);
	template<typename T> bool CompareAsciiType(LibconfigWrapper *lcw, ParBase *curr);

	bool CompareAsciiFuncType(LibconfigWrapper *lcw, ParBase *curr );
	///@} End of group started by \name
public:
	/**
	 * \name Constructor and Destructor
	 * @{ 
	 */
	ParamManager();		///< \brief Constructor. Initializes internal linked list of ParBase.
	~ParamManager();	///< \brief Destructor. Frees linked list of ParBase.
	///@} End of group started by \name

	/**
	 * \name Add variables of primitiv types to the managed parameter set
	 * @{ 
	 */

	/**
	 * \name Add variables to the managed parameter set
	 * There are different functions for all supported primitiv types, type FuncType, 
	 * type NArray of primitive type and arrays of all primitiv types
	 *
	 * \param addr          References the variable
	 * \param cfg_name      Name in ascii config 
	 * \param cfg_optional  If true then the parameter is optional in ascii config (defaults to false)
	 * @{ 
	 */

	void Add(bool           &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(char           &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(unsigned char  &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(short          &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(unsigned short &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(int            &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(unsigned int   &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(float          &addr, char *cfg_name=0, bool cfg_optional=false);
	void Add(double         &addr, char *cfg_name=0, bool cfg_optional=false);

	void Add(FuncType      &addr, char *cfg_name=0, bool cfg_optional=false);

	template <typename T> 
	void Add(NArray<T>     &addr, char *cfg_name, bool cfg_optional=false);

	template<typename T, size_t N>
	void Add(const T ( &addr )[N], char *cfg_name=0, bool cfg_optional=false) {
		AddArray<T>(addr, cfg_name, cfg_optional, N);
	}

	template<typename T>
	void Add( T * addr, int N,  char *cfg_name=0, bool cfg_optional=false) {
		AddArray<T>(addr, cfg_name, cfg_optional, N);
	}

	///@} End of group started by \name

	/**
	 * \name Parameter grouping
	 * For the ascii file format nested grouping of parameters is supported.
	 *
	 * \param cfg_group      Group name in ascii config 
	 * @{ 
	 */
	void GroupBeg(char *cfg_group);
	void GroupEnd(char *cfg_group);
	///@} End of group started by \name

	/**
	 * \name Saving, loading and comparison of binary format files
	 *
	 * There are two versions of each method, the ones taking a filename
	 * and another taking an already open binary file allowing to combine
	 * several data streams into a single file.
	 *
	 * \param name       Filename for binary data
	 * \param f          FILE pointer of an already open file
	 * \return           TRUE on success or FALSE on error.
	 * @{ 
	 */
	bool SaveBinary(char *name);	///< Save to binary file
	bool LoadBinary(char *name);	///< Load from binary file
	bool CompareBinary(char *name);	///< Compare managed parameter set with binary file

	bool SaveBinary(FILE *f);		///< Append to binary file
	bool LoadBinary(FILE *f);		///< Read from binary file and advance filepointer
	bool CompareBinary(FILE *f);	///< Compare managed parameter set with binary file and advance filepointer
	///@} End of group started by \name

	/**
	 * \name Saving and loading of ascii files
	 * 
	 * For reading and writing of ASCII files the external libconfig is 
	 * used via the LibConfigWrapper class.
	 *
	 * In difference to the binary format, the whole file is load or
	 * saved, therefore the functions have a filename parameter
	 *	 
	 * \param name       Filename for ascii config
	 * \return           TRUE on success or FALSE on error
	 * @{ 
	 */
	bool SaveAscii(char *name);    ///< Save parameter set to ascii config file
	bool LoadAscii(char *name);    ///< Load parameter from ascii config file
	bool CompareAscii(char *name); ///< Compare managed parameter set with ascii config file
	///@} End of group started by \name


	/**
	 * \name Misc functions
	 * @{ 
	 */
	char *GetNameOfFailedParam();			 ///< Return parameter name where the load failed
	bool WasLoadedFromFile(char *paramName); ///< Returns TRUE if parameter was read from ascii config or binary file
	///@} End of group started by \name
};

#endif // PARAM_MANAGER_H