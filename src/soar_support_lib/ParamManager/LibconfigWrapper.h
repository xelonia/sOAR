/**
* \file LibconfigWrapper.h
* \brief Declaration of class LibconfigWrapper
*
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
*/

#ifndef LIB_CONFIG_WRAPPER_H
#define LIB_CONFIG_WRAPPER_H

#include "LibconfigWrapperEnums.h"
#include "libconfig.h++"

#ifdef GENERATE_DOXYGEN_DOC
#define LCW_NO_NARRAY       ///< If defined during compile time, the class NArray<T> is NOT included or supported in LibconfigWrapper
#define LCW_NO_FUNCTYPE     ///< If defined during compile time, the class FuncType  is NOT included or supported in LibconfigWrapper
#endif
// Place the above #defines after this line to suppress support of NArray or FuncType


#ifndef LCW_NO_NARRAY
template<typename T> class NArray;
#endif

/**
 * \ingroup ParamManager
 * \brief Wrappers access to the external libconfig library.
 *
 * Ascii serialization and deserialization of primitive types
 * and the SoarSupport types NArray and FuncType.
 *
 * <DIV class="groupHeader">Features</DIV>
 * - ASCII serialization and deserialization using libconfig
 * - Support for primitive types (bool, char, unsigned char, short, unsigned short, int, unsigned int, float, double)
 * - Support for arrays of all primitive types
 * - Support for FuncType 
 * - Support for NArray of primitive types with up to 10 dimensions
 * - Support of special input formats for 1D and 2D NArrays
 * - Support for grouping in ascii config
 * - Syntax checks for all types and generation of good error messages
 * - Extensions to libconfig
 *   - For boolean types allows usage of true/false or 0/1
 *   - For float/double types allows usage of int
 *   - Support for short 16 bit type
 *   - Support/tests for unsigned datatypes
 * - Decoupled from ParamManager and the other SoarSupport classes (except ParTypes.h) .<BR/>
 *   Define #LCW_NO_NARRAY or #LCW_NO_FUNCTYPE to compile without NArray and FuncType 
 * - Unit tested
 *
 * <DIV class="groupHeader">Usage</DIV>
 * \code
 *
 * void SaveConfig() {
 *    LibconfigWrapper lcw;
 *
 *    // For config saving one needs to call 'Store' for all variables 
 *    // and groups first and Save() at the end.
 *
 *    // All LibconfigWrapper functions return a boolean state to 
 *    // signal an error, the reason is then specified in the error message.
 *    bool  a = true;
 *    int   b = 1;
 *    long  c = 100000042;
 *    float d = 3.1415;
 *    char  e[32] = "HelloWorld";
 *
 *    // Save vars
 *    if ( !lcw.Store(&a,"A", T_BOOL, M_BUILDIN)   )  printf( lcw.GetLastParamErrorMsg() );
 *    if ( !lcw.Store(&b,"B", T_INT , M_BUILDIN)    )  printf( lcw.GetLastParamErrorMsg() );
 *    if ( !lcw.Store(&c,"C", T_LONG, M_BUILDIN)   )  printf( lcw.GetLastParamErrorMsg() );
 *
 *    // Now save vars to group  
 *    if ( !lcw.GroupBeg("GroupName");             )  printf( lcw.GetLastParamErrorMsg() );
 *    if ( !lcw.Store(&d,"D", T_FLOAT, M_BUILDIN)  )  printf( lcw.GetLastParamErrorMsg() );
 *    // For a M_STRING, the type does not matter
 *    if ( !lcw.Store(&e,"E",  T_INT, M_STRING)    )  printf( lcw.GetLastParamErrorMsg() ); 
 *    if ( !lcw.GroupEnd("GroupName")              )  printf( lcw.GetLastParamErrorMsg() );
 *
 *    // For config saving one needs to Store() all vars first and then Save()
 *    if ( !lcw.Save("MyConfig")                   )  printf( lcw.GetLastParamErrorMsg() );
 * }
 *
 * void LoadConfig() {
 *    LibconfigWrapper lcw;
 *
 *    // All LibconfigWrapper functions return a boolean state to 
 *    // signal an error, the reason is then specified in the error message.
 *
 *    // For config loading one needs to Load() first and then Parse() all vars
 *    if ( !lcw.Load("MyConfig") )  {
 *        printf( lcw.GetLastParamErrorMsg() );
 *        return;
 *    }
 *
 *    bool  a
 *    int   b;
 *    long  c;
 *    float d;
 *    char  e[32];
 *
 *    // Load vars
 *    if ( !lcw.Parse(&a,"A", T_BOOL, M_BUILDIN)    ) printf( lcw.GetLastParamErrorMsg() );
 *    if ( !lcw.Parse(&b,"B", T_INT , M_BUILDIN)     ) printf( lcw.GetLastParamErrorMsg() );
 *    if ( !lcw.Parse(&c,"C", T_LONG, M_BUILDIN)    ) printf( lcw.GetLastParamErrorMsg() );
 *
 *    // Load vars from group
 *    if ( !lcw.GroupBeg("GroupName")               ) printf( lcw.GetLastParamErrorMsg() );
 *    if ( !lcw.Parse(&d,"D", T_FLOAT, M_BUILDIN)   ) printf( lcw.GetLastParamErrorMsg() );
 *
 *    // For a M_STRING, maxLen is needed and the T type does not matter
 *    if ( !lcw.Parse(&e,"E",  T_INT , M_STRING, 32) ) printf( lcw.GetLastParamErrorMsg() ); 
 *    if ( !lcw.GroupEnd("GroupName")               ) printf( lcw.GetLastParamErrorMsg() );
 * }
 *    
 * \endcode
 *
 * <DIV class="groupHeader">Versions</DIV>
 * \code
 *   27.07.2017 Added GetLastParamFoundInCfgFile()to support error messages for optional parameters
 *   02.04.2016 Condensed templates, reduced number of functions
 *   16.03.2016 Added support for char and uchar as base types, array types and NArray types
 *   15.03.2016 Added support for arrays of arbitary size
 *   06.03.2016 Added support for strings
 *   05.03.2016 Added support for grouping
 *   04.03.2016 Added serialization of all types
 *   02.03.2016 Initial version, supports only loading
 * \endcode
 *
 * <DIV class="groupHeader">Copyright</DIV>
 * (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
 * <BR/>
* <HR />
*
* <H2 class="groupheader">Include</H2>
 */
class LibconfigWrapper
{
private:
	libconfig::Config          _cfg;					///< The config instance the CFG file is parsed into
	char                       _currNameAndType[512];	///< Stores the current parameter name and type for error messages
	char                       _currErrMsg[512];		///< Stores the current error message
	char                       _currIdxStr[512];		///< Stores the 'path' of the current parameter used by GroupBeg()/GroupEnd()
	char                       _combinedErrorMessage[1024];///< Stores the combined name,type and error message
	char                       _pathPart[10][64];		///< Stores the parts of the parameter 'path' used by GroupBeg()/GroupEnd()
	int                        _pathIdx;				///< Index into path. group depth
	bool                       _isParsing;				///< Flags for GroupBeg() if parsing or storing 
	bool					   _paramFoundInConfig;		///< Flags if last parsed parameter was found in CFG 
	
	template<typename T> bool GetVal(libconfig::Setting &set, T &v);

	template<typename T> bool ParseAndSetBuildin(libconfig::Setting &set, void *data);	
	template<typename T> bool ParseAndSetArray( libconfig::Setting &set, void *data, int arrayLen, LcwBaseType bt);
	template<typename T> bool Parse(libconfig::Setting &set, void *data, LcwBaseType bt, LcwBaseMode bm, int arrayLen);

	bool StoreBuildin(void *data, const char *name, LcwBaseType bt);
	bool StoreArray(void *data, const char *name, LcwBaseType bt, int arrayLen);

#ifndef LCW_NO_NARRAY
	void GenNArrayIdxStr(int idx[], int d);

	template<typename T> T & GetNArrayVal(NArray<T> *arr, int idx[], int d);

	template<typename T> bool ReadNArrayLevel(libconfig::Setting &parent, NArray<T> &data, int idx[], int dim[], int d, int maxD);

	template<typename T> bool ParseAndSetNArraySpecialCase1D(libconfig::Setting &set, void *data);
	template<typename T> bool ParseAndSetNArraySpecialCase2dCSV(libconfig::Setting &set, void *data);
	template<typename T> bool ParseAndSetNArray(libconfig::Setting &set, void *data);

	template<typename T> void StoreNArray(libconfig::Setting & parent, NArray<T> *arr, int idx[], int dim, bool asFloat);
	template<typename T> bool StoreNArray(libconfig::Setting & root, void *data, const char *name, bool asFloat);
#endif

#ifndef LCW_NO_FUNCTYPE
	bool ParseAndSetFuncType(libconfig::Setting &set, void *data);
	bool StoreFuncType(void *data, const char *name);
#endif

	bool CheckTypeAndModeAndStoreName(const char *name, LcwBaseType bt, LcwBaseMode bm);
	
	libconfig::Setting & GetPath();

public:
	LibconfigWrapper();

	/**
	 * Load the CFG file using libconfig++. The config data is then 
	 * stored in the _cfg variable for access with the Parse() function
	 * @param filename  The cfg filename to load
	 * @return TRUE if cfg file was loaded
	 */
	bool  Load(char *filename);

	/**
	 * Parses a single parameter. The loaded _cfg variable is checked for
	 * a parameter given by its name using the current group path and then
	 * parsed into data pointer.
	 * @param data  Pointer to storage for the data
	 * @param name  The name of the parameter to be parsed
	 * @param bt    The BaseType (bool, int, float etc...) of the parameter
	 * @param bm    The BaseMode (builtin, array, functype ...) of the parameter
	 * @return      TRUE if paramter was parsed into data. 
	 *              For FALSE use the GetLastParamFoundInCfgFile() and GetLastParamErrorMsg() 
	 *              functions to check if the parameter was found and its error
	 */
	bool  Parse(void *data, const char *name, LcwBaseType bt, LcwBaseMode bm, int arrayLen=0);	

	/**
	 * Stores a single parameter. The parameter is added to the _cfg variable
	 * using the name at the current group path.
	 * @param data     Pointer to storage of the data
	 * @param name     The name of the parameter to be stored
	 * @param bt       The BaseType (bool, int, float etc...) of the parameter
	 * @param bm       The BaseMode (builtin, array, functype ...) of the parameter
	 * @param arrayLen Optional extra parameter for arrays specifying their length
	 * @return         TRUE if added successfully, if FALSE use GetLastParamErrorMsg()
	 *                 to retrieve an error reason.
	 */
	bool  Store(void *data, const char *name, LcwBaseType bt, LcwBaseMode bm, int arrayLen=0);

	/**
	 * Saves a config file. To be called after storing all parameters to _cfg.
	 * @param filename  The cfg filename to save the _cfg into.
	 * @return TRUE if cfg file was saved
	 */
	bool  Save(char *filename);

	/**
	 * Begins a parameter group. This adds the 'name' to the current parameter path.
	 * @param name The name of the group
	 * @return     TRUE if succesfull, if FALSE use GetLastParamErrorMsg()
	 */
	bool  GroupBeg(char *name);

	/**
	 * Ends a parameter group. This changes back to the previous parameter path.
	 * @param name The name of the group
	 * @return     TRUE if succesfull, if FALSE use GetLastParamErrorMsg()
	 */
	bool  GroupEnd(char *name);

	/**
	* Returns if last parsed parameter was found in config.
	* @return TRUE if last parsed parameter was found in loaded config
	*/
	bool   GetLastParamFoundInCfgFile();

	/**
	* Returns the error message for the last parsed or stored parameter.
	* @return Error message for last parsed/stores parameter
	*/
	char  *GetLastParamErrorMsg();
};

#endif // LIB_CONFIG_WRAPPER_H