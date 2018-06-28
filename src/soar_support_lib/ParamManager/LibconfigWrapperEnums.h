/**
* \file LibconfigWrapperEnums.h
* \brief Declaration of enum LcwBaseType and LcwBaseMode
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#ifndef LIBCONFIG_WRAPPER_ENUMS_H
#define LIBCONFIG_WRAPPER_ENUMS_H

/**
* \ingroup ParamManager
* \brief Enumerates the supported primitive parameter data types
*/
enum LcwBaseType {
	T_BOOL     = 0,   ///<   8 bit boolean 
	T_CHAR     = 1,   ///<   8 bit char 
	T_UCHAR    = 2,   ///<   8 bit unsigned char 
	T_SHORT    = 3,   ///<  16 bit short
	T_USHORT   = 4,   ///<  16 bit unsigned short
	T_INT      = 5,   ///<  32 bit integer 
	T_UINT     = 6,   ///<  32 bit unsigned integer 
	T_FLOAT    = 7,   ///<  32 bit float
	T_DOUBLE   = 8,   ///<  64 bit double
};

/**
* \ingroup ParamManager
* \brief Enumerates the supported parameter variants.
*/
enum LcwBaseMode {
	M_BUILDIN   = 0,    ///<  Primitive paramater type that can be saved and loaded directly with fread/fwrite
	M_ARRAY     = 1,    ///<  Parameter of array[] type of buildin parameter type
	M_FUNCTYPE  = 2,    ///<  Parameter of FuncType class
	M_NARRAY    = 3,    ///<  Parameter of NArray of primitive type class
	M_GROUP_BEG = 4,	///<  Pseudo parameter type to start a parameter group
	M_GROUP_END = 5,	///<  Pseudo parameter type to end   a parameter group
};

#endif // LIBCONFIG_WRAPPER_ENUMS_H