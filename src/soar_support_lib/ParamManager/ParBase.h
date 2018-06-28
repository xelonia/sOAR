/**
* \file ParBase.h
* \brief Declaration and inline implementation of class ParBase, ParPrim, ParArray, ParClass and ParGroup
*
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
*/

#ifndef PAR_BASE_H
#define PAR_BASE_H

#include "LibconfigWrapperEnums.h"

/**
* \defgroup ParamManager sOAR_Support_ParamManager
* \brief This module contains the private support classes and enumerations of the ParamManager.
*
* The classes and enums are only used inside of ParamManager.cpp. The  and therefore
* not referenced or used in any classes of sOAR_Support or sOAR_Lib.
*/

/**
* \ingroup ParamManager
* \interface ParamManager::ParBase
* \brief ParamManager helper class: Abstract base class for the linked list of paramaters of a parameter set.
*
* There are four implementations of this abstract base class - ParPrim, ParArray, ParClass and ParGroup
* <HR />
*
* <H2 class="groupheader">Include</H2>
*/
class ParamManager::ParBase {
private:
	friend class ParamManager;			///< Allow access to members
#ifdef UNIT_TEST
	friend class UtParamManager;		///< Allow access to private classes for unit tests
	friend class UtLibconfigWrapper;	///< Allow access to private classes for unit tests
#endif

	ParBase *  _next;

protected:
	char  *      _name;			///< Name of the parameter
	void  *      _data;			///< Pointer to the start of memory of parameter
	LcwBaseType  _type;			///< Type of parameter (char, int, double ...)
	LcwBaseMode  _mode;			///< Mode of parameter (buildin, array, class, group...)
	bool         _opt;			///< True if parameter is optional
	bool         _readFromFile;	///< True if parameter was read parsed from file

	virtual bool SaveBinary(FILE *f)    = 0;	///< Abstract function to save the parameter in binary form
	virtual bool LoadBinary(FILE *f)    = 0;	///< Abstract function to load the parameter in binary form
	virtual bool CompareBinary(FILE *f) = 0;	///< Abstract function to compare the parameter in binary form to current value

	ParBase(void *addr, LcwBaseType bt, LcwBaseMode bm, char *name, bool opt)
		: _data(addr), _type(bt), _mode(bm), _name(name), _opt(opt), _next(0), _readFromFile(false)
	{ 
	}

	virtual ~ParBase() {}

	char *GetTypeName() {
		return GetTypeName(_type);
	}

public:
	static char *GetTypeName(LcwBaseType t) {
		char *tn[9] = { "BOOL","CHAR","UCHAR","SHORT","USHORT","INT","UINT","FLOAT","DOUBLE" };
		int   ti = (int)t;
		if (ti<0 || ti>8)
			return "<Unsupported Type>";
		return tn[ti];
	}
};


/**
* \ingroup ParamManager
* \brief  ParamManager  helper class: Class for primitive types in the parameter set.
*
* The binary load,save and compare functions for primitive types
* are implemented directly using fwrite() and fread() and the size of 
* the data type.
*/

class ParamManager::ParPrim : public ParBase {
private:
	friend class ParamManager;			///< Allow access to members
#ifdef UNIT_TEST
	friend class UtParamManager;		///< Allow access to private classes for unit tests
	friend class UtLibconfigWrapper;	///< Allow access to private classes for unit tests
#endif

	int  _dataSize;

	bool SaveBinary(FILE *file)		{ 	return fwrite(_data, _dataSize,1, file)==1;		}
	bool LoadBinary(FILE *file)		{	return fread( _data, _dataSize,1, file)==1;		}
	bool CompareBinary(FILE *file)	
	{	
		char tmp[32]; // Longer than longest basetype
		if (fread(&tmp, _dataSize, 1, file) != 1)
			return false;
		return (memcmp(_data, &tmp, _dataSize)==0);		
	}

	ParPrim(void *a, LcwBaseType t, char *n, bool o, int size) : ParBase(a,t,M_BUILDIN,n,o), _dataSize(size)	{}
};


/**
* \ingroup ParamManager
* \brief ParamManager  helper class: Class for array types in the parameter set.
*
* The binary load,save and compare functions for the array type 
* save/read the array length and then the array data
*/
class ParamManager::ParArray : public ParBase {
private:
	friend class ParamManager;			///< Allow access to members
#ifdef UNIT_TEST
	friend class UtParamManager;		///< Allow access to private classes for unit tests
	friend class UtLibconfigWrapper;	///< Allow access to private classes for unit tests
#endif

	int  _maxLen;
	int  _dataSize;

	bool SaveBinary(FILE *file)		{	
		if ( fwrite(&_maxLen, sizeof(_maxLen),1, file)!=1 )
			return false;
		
		return fwrite(_data, _dataSize, _maxLen, file)==_maxLen;
	}

	bool LoadBinary(FILE *file)		{	
		int len;
		if ( fread(&len, sizeof(len),1, file)!=1 )
			return false;

		if (len != _maxLen) 
			return false;

		return fread(_data, _dataSize, _maxLen, file) == _maxLen;
	}

	bool CompareBinary(FILE *file)	{
		int  len;	

		if ( fread(&len, sizeof(len),1, file)!=1 )
			return false;

		if ( len != _maxLen)
			return false;

		int   length = _dataSize * _maxLen;
		char * bytes = new char[length];	

		if ( fread(bytes, 1, length, file)!=length ) {
			delete [] bytes;
			return false;
		}

		bool ret = memcmp(bytes, _data, length) == 0;
		delete [] bytes;

		return ret;
	}

	ParArray(void *a, LcwBaseType t, LcwBaseMode m, char *n, bool o,int max, int size) 
		: ParBase(a,t,m,n,o),_maxLen(max),_dataSize(size) {}
};

/**
* \ingroup ParamManager
* \brief ParamManager  helper class: Template class for class types in the parameter set.
*
* The binary load,save and compare functions for class types
* are implemented using SaveBinary(), LoadBinary() and CompareBinary()
* member functions of the referenced classes.
*/
template<class T> class ParamManager::ParClass : public ParamManager::ParBase {
private:
	friend class ParamManager;			///< Allow access to members
#ifdef UNIT_TEST
	friend class UtParamManager;		///< Allow access to private classes for unit tests
	friend class UtLibconfigWrapper;	///< Allow access to private classes for unit tests
#endif

	bool SaveBinary(FILE *file)		{	return ((T *)_data)->SaveBinary(file);		}
	bool LoadBinary(FILE *file)		{	return ((T *)_data)->LoadBinary(file);		}
	bool CompareBinary(FILE *file)	{	return ((T *)_data)->CompareBinary(file);	}

	ParClass(void *a, LcwBaseType t, LcwBaseMode m, char *n, bool o) : ParBase(a,t,m,n,o)		{}
};

/**
* \ingroup ParamManager
* \brief ParamManager  helper class: Class for pseudo parameter types used for grouping
*
* The binary load,save and compare functions for the pseudo parameter type 
* group types just return success - group information is only used in the ascii
* format.
*/
class ParamManager::ParGroup : public ParBase {
private:
	friend class ParamManager;			///< Allow access to members
#ifdef UNIT_TEST
	friend class UtParamManager;		///< Allow access to private classes for unit tests
	friend class UtLibconfigWrapper;	///< Allow access to private classes for unit tests
#endif

	bool SaveBinary(FILE *file)		{	return true; }
	bool LoadBinary(FILE *file)		{	return true; }
	bool CompareBinary(FILE *file)	{	return true; }

	ParGroup(LcwBaseMode m, char *n) : ParBase(0,T_BOOL,m,n,false)		{}
};


#endif // PAR_BASE_H