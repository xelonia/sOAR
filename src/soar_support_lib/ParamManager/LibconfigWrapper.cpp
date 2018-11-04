/**
* \file LibconfigWrapper.cpp
* \brief Implementation of class LibconfigWrapper
*
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
*/

#include "LibconfigWrapper.h"

#ifndef LCW_NO_NARRAY
#include "..\NArray.h"
#endif

#ifndef LCW_NO_FUNCTYPE
#include "..\FuncType.h"
#endif

LibconfigWrapper::LibconfigWrapper() {
	_pathIdx   = -1;
	_isParsing = false;
	
	_currErrMsg[0]           = 0;
	_currNameAndType[0]      = 0;
	_combinedErrorMessage[0] = 0;
	_paramFoundInConfig      = false;
}

bool  LibconfigWrapper::GetLastParamFoundInCfgFile()
{
	return _paramFoundInConfig;
}

char *LibconfigWrapper::GetLastParamErrorMsg() {
	if (_currErrMsg[0]==0)
	{
		_combinedErrorMessage[0] = 0;
	}
	else
	{
		strcpy_s(_combinedErrorMessage, _currNameAndType);
		strcat_s(_combinedErrorMessage, _currErrMsg);
	}
	return _combinedErrorMessage;
}


bool LibconfigWrapper::Load(char *configName)
{
	_currNameAndType[0] = 0;
	_currErrMsg[0]      = 0;
	_paramFoundInConfig = false;
	_isParsing          = true;

	try
	{
		_cfg.readFile(configName);
		return true;
	}
	catch(const libconfig::FileIOException &fioex)
	{
		sprintf_s(_currErrMsg,"File not found '%s' (%s)",configName,fioex.what());
	}
	catch(const libconfig::ParseException &pex)
	{
		sprintf_s(_currErrMsg,"Parse error in file '%s' line %d",pex.getFile(),pex.getLine() );
	}

	return false;
}

bool LibconfigWrapper::CheckTypeAndModeAndStoreName(const char *name, LcwBaseType bt, LcwBaseMode bm)
{
	char *typeName[9] = { "BOOL","CHAR","UCHAR","SHORT","USHORT","INT","UINT","FLOAT","DOUBLE" };
	char  currType[128];

	if ((int)bt<0 || (int)bt>8 ) {
		sprintf_s(_currErrMsg,"  Unsupported BaseType=%d for '%s'",bt,name);
		return false;
	}
	if ((int)bm<0 || (int)bm>5 ) {
		sprintf_s(_currErrMsg,"  Unsupported BaseMode=%d for '%s'",bm,name);
		return false;
	}

	if (bm==M_BUILDIN) {
		strcpy_s(currType,typeName[bt]);		
	}
	else if (bm==M_FUNCTYPE) {
#ifdef LCW_NO_FUNCTYPE
		strcpy_s(_currErrMsg,"Store(): FuncType support not compiled, #undefine LCW_NO_FUNCTYPE");
		return false;	
#else
		strcpy_s(currType,"FuncType");	
#endif
	}
	else if (bm==M_NARRAY) {
#ifdef LCW_NO_NARRAY
		strcpy_s(_currErrMsg,"Store(): NArray<T> support not compiled, #undefine LCW_NO_NARRAY");
		return false;
#else
		sprintf_s(currType,"NArray<%s>",typeName[bt]);
#endif
	}
	else if (bm==M_ARRAY) {
		sprintf_s(currType,"%s[]",typeName[bt]);
	}
	else if (bm==M_GROUP_BEG) {
		strcpy_s(currType,"GroupBeg");		
	}
	else if (bm==M_GROUP_END) {
		strcpy_s(currType,"GroupEnd");		
	}

	sprintf_s(_currNameAndType,"  %-14s %-16s : ",currType, name);

	// Initialize parameter error messages
	_currErrMsg[0]           = 0;
	_combinedErrorMessage[0] = 0;
	_paramFoundInConfig	     = false;
	return true;
}

template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, bool &b)
{
	if (set.getType()==libconfig::Setting::TypeInt) {
		int iVal = set;

		if (iVal!=0 && iVal!=1) {
			strcpy_s(_currErrMsg,"Only 0 or 1, true or false are allowed");
			return false;
		}

		b = (iVal!=0);
		return true;
	}
	else if (set.getType()==libconfig::Setting::TypeBoolean) {
		b = set;		
		return true;
	}

	strcpy_s(_currErrMsg,"Only 0 or 1, true or false are allowed");
	return false;
}


template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, char &c)
{
	if (set.getType()==libconfig::Setting::TypeInt) {
		int i = set;
		if (i<-128 || i>127) {
			strcpy_s(_currErrMsg,"Out of range -128 .. +127");
			return false;
		}
		c = (char)i;
		return true;
	}
	if (set.getType()==libconfig::Setting::TypeString) {		
		const char *str = set;
		int len = strlen(str);
		if (len!=1) {
			strcpy_s(_currErrMsg,"Only 1 character string are supported");
			return false;
		}
		c = str[0];
		return true;
	}
	strcpy_s(_currErrMsg,"Only number in -128..+127 or 1 character string are supported");
	return false;
}

template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, unsigned char &c)
{
	if (set.getType()==libconfig::Setting::TypeInt) {
		int i = set;
		if (i<0 || i>255) {
			strcpy_s(_currErrMsg,"Out of range 0..255");
			return false;
		}
		c = (unsigned char)i;
		return true;
	}
	if (set.getType()==libconfig::Setting::TypeString) {		
		const char *str = set;
		int len = strlen(str);
		if (len!=1) {
			strcpy_s(_currErrMsg,"Only 1 character string are supported");
			return false;
		}
		c = (unsigned char)str[0];
		return true;
	}
	strcpy_s(_currErrMsg,"Only number in 0..+255 or 1 character string are supported");
	return false;
}


template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, short &s)
{	
	if (set.getType()==libconfig::Setting::TypeInt) {
		int ii = set;
		if (ii<-32768 || ii>32767) {
			strcpy_s(_currErrMsg,"Value out of range -32768..+32767");
			return false;
		}
		s = (short)(ii);
		return true;
	}

	strcpy_s(_currErrMsg,"Not of type integer");
	return false;
}

template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, unsigned short &s)
{
	if (set.getType()==libconfig::Setting::TypeInt) {
		int ii = set;
		if (ii<0) {
			strcpy_s(_currErrMsg,"Negative values not allowed");
			return false;
		}
		else if (ii>0xffff) {
			strcpy_s(_currErrMsg,"Value too large");
			return false;
		}
		s = (unsigned short)(ii);
		return true;
	}

	strcpy_s(_currErrMsg,"Not of type integer");
	return false;
}

template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, int &i)
{
	if (set.getType()!=libconfig::Setting::TypeInt) {
		strcpy_s(_currErrMsg,"Not of type integer");
		return false;
	}

	i = set;
	return true;
}

template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, unsigned int &i)
{
	if (set.getType()==libconfig::Setting::TypeInt64) {
		long long ll = set;
		if (ll<0) {
			strcpy_s(_currErrMsg,"Negative values not allowed");
			return false;
		}
		i = (unsigned int)(ll);
		return true;		
	}
	if (set.getType()==libconfig::Setting::TypeInt) {
		int ii = set;
		if (ii<0) {
			strcpy_s(_currErrMsg,"Negative values not allowed");
			return false;
		}
		i = (unsigned int)(ii);
		return true;
	}

	strcpy_s(_currErrMsg,"Not of type integer");
	return false;
}


template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, float &f)
{
	if (set.getType()==libconfig::Setting::TypeInt) {
		int iVal  = set;
		f = (float)iVal;
		return true;
	}
	else if (set.getType()==libconfig::Setting::TypeFloat) {
		f = set;
		return true;
	}

	strcpy_s(_currErrMsg,"Not of type floating point or integer number");
	return false;
}

template <> bool LibconfigWrapper::GetVal(libconfig::Setting &set, double &d)
{
	if (set.getType()==libconfig::Setting::TypeInt) {
		int iVal  = set;
		d = (double)iVal;
		return true;
	}
	else if (set.getType()==libconfig::Setting::TypeFloat) {
		d  = set;
		return true;
	}

	strcpy_s(_currErrMsg,"Not of type floating point or integer number");
	return false;
}

#ifndef LCW_NO_FUNCTYPE
bool LibconfigWrapper::ParseAndSetFuncType(libconfig::Setting &set, void *data)
{
	// parse int constant values into constant FuncType
	if (set.getType()==libconfig::Setting::TypeInt) {
		int iVal  = set;
		((FuncType *)data)->SetConstant( iVal );				
		return true;
	}

	// parse float constant values into constant FuncType
	if (set.getType()==libconfig::Setting::TypeFloat) {
		((FuncType *)data)->SetConstant( set );				
		return true;
	}

	// All FuncTypes are stored as list with two entries
	if (set.getType()!=libconfig::Setting::TypeList || set.getLength() != 2) {
		strcpy_s(_currErrMsg,"Needs to be a list with 2 entries, a string and a nested list" );
		return false;
	}

	libconfig::Setting &par0 = set[0];	
	libconfig::Setting &par1 = set[1];	

	// The first entry needs to be a string, the second one a list of parameters
	if (   par0.getType()!=libconfig::Setting::TypeString 
		|| par1.getType()!=libconfig::Setting::TypeList ){
		strcpy_s(_currErrMsg,"Needs to be a list with 2 entries, a string and a nested list" );
		return false;
	}

	// Check functype name vs list of allowed names
	const char *sVal = par0;
	int pCnt = par1.getLength();

	char *funcName[7] = { "Constant","Linear","Quadratic","Hyperbolic","Sigmoid","Exponential","Dy" };
	int ft = -1;
	for(int i=0; i<7; i++) {
		if (_stricmp(sVal,funcName[i])==0) {
			ft = i;
			break;
		}
	}

	if (ft<0) {
		strcpy_s(_currErrMsg,"Unsupported func type" );
		return false;
	}

	// The number of needed parameters depends on the function
	if (ft==0 && pCnt!=1) {
		strcpy_s(_currErrMsg,"This func type needs exactly one double parameter" );
		return false;
	}
	else if (ft==2 && pCnt!=3) {
		strcpy_s(_currErrMsg,"This func type needs exactly three double parameter" );
		return false;
	}
	else if (ft!=0 && ft!=2 && pCnt!=2) {
		strcpy_s(_currErrMsg,"This func type needs exactly two double parameter" );
		return false;
	}

	// Prepare empty parameters as zero
	double a[3];
	for(int i=0;i<3;i++) {		
		a[i] = 0;
	}

	// Read all supplied parameters into array a[]
	for(int i=0;i<pCnt;i++) {		
		libconfig::Setting &parV = par1[i];

		if (parV.getType()!=libconfig::Setting::TypeInt && parV.getType()!=libconfig::Setting::TypeFloat) {
			strcpy_s(_currErrMsg,"The parameter list needs to contain floating point values" );
			return false;
		}

		GetVal<double>(parV, a[i]); // Type is already checked
	}

	// Initialize func type with function and paramaters, ft range is alreaxdy checked above
	switch(ft) {
	default:  // default case should never happen
	case 0:  ((FuncType *)data)->SetConstant(a[0]);				return true;
	case 1:  ((FuncType *)data)->SetLinear(a[0],a[1]);			return true;
	case 2:  ((FuncType *)data)->SetQuadratic(a[0],a[1],a[2]);	return true;
	case 3:  ((FuncType *)data)->SetHyperbolic(a[0],a[1]);		return true;
	case 4:  ((FuncType *)data)->SetSigmoid(a[0],a[1]);			return true;
	case 5:  ((FuncType *)data)->SetExponential(a[0],a[1]);		return true;
	case 6:  ((FuncType *)data)->SetDy(a[0],a[1]);				return true;
	}
}

bool  LibconfigWrapper::StoreFuncType(void *data, const char *name) 
{
	libconfig::Setting & root = GetPath();

	libconfig::Setting & ftSet = root.add(name, libconfig::Setting::TypeList);
	FuncType                   *ft = (FuncType *)data;
	FuncType::FuncTypeVariant  ftv = ft->GetType();
	char                  *funcName;

	switch(ftv) {
	default: // default case should never happen
	case FuncType::FT_NONE: sprintf_s(_currErrMsg,"type=%d not supported", ftv);	return false;

	case FuncType::FT_ZERO: 
	case FuncType::FT_ONE: 
	case FuncType::FT_CONSTANT:    funcName = "Constant";    break;
	case FuncType::FT_LINEAR:      funcName = "Linear";      break; 
	case FuncType::FT_QUADRATIC:   funcName = "Quadratic";   break; 	
	case FuncType::FT_HYPERBOLIC:  funcName = "Hyperbolic";  break; 	
	case FuncType::FT_SIGMOID:     funcName = "Sigmoid";     break; 	
	case FuncType::FT_EXPONENTIAL: funcName = "Exponential"; break; 		
	case FuncType::FT_DY:          funcName = "Dy";          break; 		
	}	
	// Add a funcname to list
	ftSet.add(libconfig::Setting::TypeString) = funcName;

	// Add list of parameters
	libconfig::Setting & ftPar = ftSet.add(libconfig::Setting::TypeList);
	ftPar.add(libconfig::Setting::TypeFloat) = ft->GetParam(0);

	if (ftv!=FuncType::FT_ZERO && ftv!=FuncType::FT_ONE && ftv!=FuncType::FT_CONSTANT) {
		ftPar.add(libconfig::Setting::TypeFloat) = ft->GetParam(1);

		if (ftv==FuncType::FT_QUADRATIC) {
			ftPar.add(libconfig::Setting::TypeFloat) = ft->GetParam(2);
		}
	}
	return true;
}

#endif // !LCW_NO_FUNCTYPE

template<typename T> bool LibconfigWrapper::ParseAndSetBuildin(libconfig::Setting &set, void *data)
{
	T val;
	if (!GetVal<T>(set, val) )
		return false;
	*(T *)data = val;
	return true;
}

#ifndef LCW_NO_NARRAY
template<typename T> bool LibconfigWrapper::ParseAndSetNArraySpecialCase1D(libconfig::Setting &set, void *data)
{	
	if (set.getType()==libconfig::Setting::TypeList) {
		int cnt = set.getLength();

		// Check for empty array
		if (cnt<1) {
			NArray<T> arr;
			*(NArray<T>*)data = arr;
			return true;
		}

		bool ok = true;

		T  *val = new T[cnt];

		for(int i=0; i<cnt; i++) {
			libconfig::Setting &par = set[i];
			if ( !GetVal<T>(par, val[i]) )
				ok = false;
		}

		if (ok) {
			NArray<T> arr1D;

			arr1D.Init(cnt);
			for(int i=0; i<cnt; i++)
				arr1D[i] = val[i];

			*(NArray<T>*)data = arr1D;
		}
		delete [] val;


		if (ok)
			return true;
	}
	return false;
}

template<typename T> bool LibconfigWrapper::ParseAndSetNArraySpecialCase2dCSV(libconfig::Setting &set, void *data)
{
	// Function is only called if set.getType()==TypeString
	// Check if string is a filename that can be openend
	std::string filename = set;

	if (filename.c_str()==NULL || filename.c_str()[0]==0) {
		sprintf_s(_currErrMsg,"Empty CSV file name");
		return false;
	}

	FILE *file;
	fopen_s(&file,filename.c_str(),"r"); 
	if (file==0) {			
		sprintf_s(_currErrMsg,"Could not open '%s' as CSV file",filename.c_str() );
		return false;
	}

	// Each line needs to contain the index as first value, followed by up to 10 floats 
	char lineBuf[2048];
	int  line = 0;
	int  parCnt = -1;
	bool sepComma = false;

	// The first parsing of the file does all the error and format checking
	// and find the width of both dimensions (lineCnt-1 x parCnt-1)
	while( fgets(lineBuf, sizeof(lineBuf), file) != NULL) {
		float        val[10];
		unsigned int idx;

		// First line is skipped
		if (line>0) {
			if (line==1) {
				int cntC = sscanf_s(lineBuf, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",&idx,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6],&val[7],&val[8],&val[9]);
				int cntS = sscanf_s(lineBuf, "%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f",&idx,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6],&val[7],&val[8],&val[9]);
				sepComma = cntC > cntS;
			}
		
			unsigned int cnt;
			if (sepComma) {
				cnt = sscanf_s(lineBuf, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",&idx,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6],&val[7],&val[8],&val[9]);
			}
			else {
				cnt = sscanf_s(lineBuf, "%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f",&idx,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6],&val[7],&val[8],&val[9]);
			}

			if (parCnt<0)		// Initialize parameter count from line#1
				parCnt = cnt;

			if (cnt<2) {
				sprintf_s(_currErrMsg,"In the referenced CSV file '%s', line %d contains only one parameter, probably separator is wrong",filename.c_str(),line );
				fclose(file);
				return false;
			}

			if (cnt!=parCnt) {
				sprintf_s(_currErrMsg,"In the referenced CSV file '%s', line %d contains a different number of values than the line(s) before",filename.c_str(),line );
				fclose(file);
				return false;
			}

			if (idx!=line) {
				sprintf_s(_currErrMsg,"In the referenced CSV file '%s', line %d contains a different number of values than the line(s) before",filename.c_str(),line );
				fclose(file);
				return false;
			}

		}
		line++;
	}

	// Rewind the file and reRead all entries into the array
	fseek(file,0,SEEK_SET);

	NArray<T> arr;
	arr.Init(line-1,parCnt-1); 

	line = 0;
	while( fgets(lineBuf, sizeof(lineBuf), file) != NULL) {
		float        val[10];
		unsigned int idx;
		unsigned int cnt;

		if (sepComma) {
			cnt = sscanf_s(lineBuf, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",&idx,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6],&val[7],&val[8],&val[9]);
		}
		else {
			cnt = sscanf_s(lineBuf, "%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%f",&idx,&val[0],&val[1],&val[2],&val[3],&val[4],&val[5],&val[6],&val[7],&val[8],&val[9]);
		}
		// First line is skipped
		if (line>0) {
			// Fill the array
			for(unsigned int c = 0; c<cnt-1; c++) {

#pragma warning( disable : 4800 )	// 'float' : forcing value to bool 'true' or 'false' (performance warning)  if compiled for T=bool

				arr(idx-1, c) = (typename T) val[c];  // Uses conversion from float to type T

#pragma warning( default : 4800 )
			}
		}
		line++;
	}

	fclose(file);

	*(NArray<T>*)data = arr;
	return true;
}


void LibconfigWrapper::GenNArrayIdxStr(int idx[], int d)
{
	strcpy_s(_currIdxStr,"[");
	for(int i=0; i<d; i++) {
		char tmpStr[256];
		sprintf_s(tmpStr,"%d",idx[i]+1);
		if (i>0)
			strcat_s(_currIdxStr,",");
		strcat_s(_currIdxStr,tmpStr);
	}
	strcat_s(_currIdxStr,"]");
}

template<typename T> T &LibconfigWrapper::GetNArrayVal(NArray<T> *arr, int idx[], int d)
{
	switch(arr->GetDims() ) 
	{
	default: // Never choosen, since the dimensionality range 1-10 is already tested in ParseAndSetNArray()
	case 1: return (*arr)(d);
	case 2: return (*arr)(idx[0],d);
	case 3: return (*arr)(idx[0],idx[1],d); 
	case 4: return (*arr)(idx[0],idx[1],idx[2],d);
	case 5: return (*arr)(idx[0],idx[1],idx[2],idx[3],d); 
	case 6: return (*arr)(idx[0],idx[1],idx[2],idx[3],idx[4],d); 
	case 7: return (*arr)(idx[0],idx[1],idx[2],idx[3],idx[4],idx[5],d);
	case 8: return (*arr)(idx[0],idx[1],idx[2],idx[3],idx[4],idx[5],idx[6],d); 
	case 9: return (*arr)(idx[0],idx[1],idx[2],idx[3],idx[4],idx[5],idx[6],idx[7],d); ;
	case 10:return (*arr)(idx[0],idx[1],idx[2],idx[3],idx[4],idx[5],idx[6],idx[7],idx[8],d); 
	}
}

template<typename T> bool LibconfigWrapper::ReadNArrayLevel(libconfig::Setting &parent, NArray<T> &arr, int idx[], int dim[], int d, int maxD)
{
	if (parent.getLength() != dim[d] ) {
		GenNArrayIdxStr(idx,d);	
		sprintf_s(_currErrMsg, "At %s the list has %d instead of %d entries\n",_currIdxStr,parent.getLength(),dim[d]);
		return false;
	}

	for(int step=0; step<dim[d]; step++) {
		libconfig::Setting &curr = parent[step];		
		idx[d] = step;

		if (d<maxD-1) {
			if (!ReadNArrayLevel(curr, arr, idx, dim, d+1, maxD) )
				return false;
		}
		else {
			T value;

			if (!GetVal<T>(curr,value) ) {
				GenNArrayIdxStr(idx,d);	
				strcat_s(_currErrMsg," at ");
				strcat_s(_currErrMsg,_currIdxStr);
				return false;
			}

			GetNArrayVal<T>(&arr,idx,step) = value;
		}
	}
	return true;
}



template<typename T> bool LibconfigWrapper::ParseAndSetNArray(libconfig::Setting &set, void *data)
{
	// Check for special 1D case list of floats/ints  same types , or empty NArray
	if (ParseAndSetNArraySpecialCase1D<typename T>(set,data) ){
		return true;
	}

	// Check for special case of CSV import 2D NArray (works only for double, float, int, uint)
	if (set.getType()==libconfig::Setting::TypeString) {
		return ParseAndSetNArraySpecialCase2dCSV<typename T>(set,data);
	}

	if (set.getType()!=libconfig::Setting::TypeList || set.getLength() != 2) {
		strcpy_s(_currErrMsg,"Needs to be a list with 2 entries, a list of dimensions and a list with values" );
		return false;
	}

	
	libconfig::Setting &par0 = set[0];	
	libconfig::Setting &par1 = set[1];	

	if (   par0.getType()!=libconfig::Setting::TypeList 
		|| par1.getType()!=libconfig::Setting::TypeList ){		
		strcpy_s(_currErrMsg,"Needs to be a list with 2 entries, a list of dimensions and a list with values" );
		return false;
	}


	int idx[20],dim[20],dims;

	// Read dimensions etc
	dims   = par0.getLength();
	if (dims <1 || dims>10) {
		strcpy_s(_currErrMsg,"Only 1-10 dimensions supported" );
		return false;
	}
	for(int i=0;i<10;i++)
		dim[i] = 0;


	for(int i=0; i<dims; i++) {		
		libconfig::Setting &parD = par0[i];
		if ( !GetVal<int>(parD, dim[i]) || dim[i]<1) {
			strcpy_s(_currErrMsg,"All dimension need to be integer values >0" );
			return false;
		}
		idx[i] = 0;
	}

	NArray<T> arr;
	arr.Init(dim[0],dim[1],dim[2],dim[3],dim[4],dim[5],dim[6],dim[7],dim[8],dim[9]);

	if (!ReadNArrayLevel(par1, arr, idx,dim,0,dims) )
		return false;

	*(NArray<T>*)data = arr;
	return true;
}

template<typename T> void LibconfigWrapper::StoreNArray(libconfig::Setting & parent, NArray<T> *arr, int idx[], int dim, bool asFloat)
{
	if (dim== arr->GetDims()-1 ) {
		for(unsigned int d=0; d<arr->GetDim(dim); d++) {
			if (asFloat)
				parent.add(libconfig::Setting::TypeFloat) = (double) GetNArrayVal<T>(arr,idx,d);
			else
				parent.add(libconfig::Setting::TypeInt)   = (int) GetNArrayVal<T>(arr,idx,d);
		}
	}
	else {
		for(unsigned int d=0; d<arr->GetDim(dim); d++) {
			idx[dim] = d;
			libconfig::Setting & curr = parent.add(libconfig::Setting::TypeList);
			StoreNArray( curr, arr, idx, dim+1,asFloat);
		}
	}
}

template<typename T> bool LibconfigWrapper::StoreNArray(libconfig::Setting & root, void *data, const char *name, bool asFloat)
{	
	libconfig::Setting & arrList = root.add(name,libconfig::Setting::TypeList);
	NArray<T> *arr = (NArray<T> *)data;

	// Special case of empty array
	if (arr->GetDims()==0) {
		return true;
	}

	// Special case of 1D array
	if (arr->GetDims()==1) {
		for(unsigned int i=0; i< arr->GetDim(0); i++) {
			if (asFloat)
				arrList.add(libconfig::Setting::TypeFloat) = (double)(*arr)(i);
			else
				arrList.add(libconfig::Setting::TypeInt)   = (int)(*arr)(i);
		}
		return true;
	}

	// Save list of dimensions
	libconfig::Setting & dimList = arrList.add(libconfig::Setting::TypeList);
	for(unsigned int i=0;i<arr->GetDims(); i++) {
		dimList.add(libconfig::Setting::TypeInt) = (int) arr->GetDim(i);
	}
	
	int idx[10];
	for(int i=0; i<10; i++)
		idx[i] = 0;

	libconfig::Setting & valList = arrList.add(libconfig::Setting::TypeList);
	StoreNArray(valList, arr, idx, 0, asFloat);

	return true;
}
#endif // !LCW_NO_NARRAY



template<typename T> bool LibconfigWrapper::ParseAndSetArray(libconfig::Setting &set, void *data, int arrayLen, LcwBaseType bt)
{
	if (set.getType()!=libconfig::Setting::TypeList) {
		if (bt==T_CHAR || bt==T_UCHAR) {
			if (set.getType()==libconfig::Setting::TypeString) {
				// Clear the destination - T is either CHAR or UCHAR, both are 8 bits
				memset(data,0,arrayLen);

				const char *str = set;

				int len = strlen(str);
				if (len>arrayLen) {
					strcpy_s(_currErrMsg,"String longer than source char[]");
					return false;
				}

				strcpy_s( (char *)data, arrayLen, str);
				return true;
			}
			strcpy_s(_currErrMsg,"Not of type string");
			return false;
		}
		sprintf_s(_currErrMsg,"Is not a list with %d entries",arrayLen );
		return false;
	}

	// Check array size
	int len = set.getLength();
	if (len!=arrayLen) {
		sprintf_s(_currErrMsg,"List length %d differs from definition %d",len,arrayLen );
		return false;
	}

	T *theData = (T *)data;
	for(int i=0; i<len; i++) {
		libconfig::Setting &curr = set[i];
		if (!GetVal<T>(curr, theData[i]) ) {
			// Error filled by GetVal
			return false;
		}
	}

	return true;
}


template<typename T> bool LibconfigWrapper::Parse(libconfig::Setting &set, void *data, LcwBaseType bt, LcwBaseMode bm, int arrayLen) {
	// bm value range is checked by CheckTypeAndModeAndStoreName()
	switch(bm) 
	{
	default:   // default case should never happen
	case M_BUILDIN: 	return ParseAndSetBuildin<T>(set, data);
	case M_ARRAY:   	return ParseAndSetArray<T>(set, data, arrayLen,bt);
#ifndef LCW_NO_NARRAY
	case M_NARRAY:		return ParseAndSetNArray<T>(set, data);
#endif
	}
}

bool LibconfigWrapper::Parse(void *data, const char *name, LcwBaseType bt, LcwBaseMode bm, int arrayLen)
{
	// Setup type and name for debugging output, catches unknown BaseType or BaseMode
	if (!CheckTypeAndModeAndStoreName(name, bt, bm) ) {
		return false;
	}

	// The approach of using the return variable 'result' was choosen to archive 100% coverage 
	// by the unit tests. If the switch inside the try{} block returns directly, then the 
	// closing bracket of the try block is never reached by the coverage test.
	bool result = false; 

	try {	
		const libconfig::Setting & root = GetPath();

		// Throws an exception if config parameter not found
		libconfig::Setting &set = root[(const char *)name];				

		// No exception, so the parameter was found
		_paramFoundInConfig      = true;

#ifndef LCW_NO_FUNCTYPE
		if (bm==M_FUNCTYPE) {
			return ParseAndSetFuncType(set, data);
		}
#endif

		// bt value range is checked by CheckTypeAndModeAndStoreName()
		switch(bt) 
		{
		default:   // default case should never happen
		case T_BOOL:   result = Parse<bool          >(set, data, bt, bm, arrayLen); break;
		case T_CHAR:   result = Parse<char          >(set, data, bt, bm, arrayLen); break;
		case T_UCHAR:  result = Parse<unsigned char >(set, data, bt, bm, arrayLen); break;	
		case T_SHORT:  result = Parse<short         >(set, data, bt, bm, arrayLen); break;
		case T_USHORT: result = Parse<unsigned short>(set, data, bt, bm, arrayLen); break;	
		case T_INT:    result = Parse<int           >(set, data, bt, bm, arrayLen); break;
		case T_UINT:   result = Parse<unsigned int  >(set, data, bt, bm, arrayLen); break;	
		case T_FLOAT:  result = Parse<float         >(set, data, bt, bm, arrayLen); break;		
		case T_DOUBLE: result = Parse<double        >(set, data, bt, bm, arrayLen); break;		
		}
	}
	catch(const libconfig::SettingNotFoundException &nfex) {
		sprintf_s(_currErrMsg,"Not found (%s)",nfex.what());
	}
	return result;
}


bool  LibconfigWrapper::Save(char *filename)
{
  try
  {
    _cfg.writeFile(filename);
	return true;
  }
  catch(const libconfig::FileIOException &fioex)
  {
	  sprintf_s(_currErrMsg,"Error writing file'%s' (%s)",filename,fioex.what());
  }
  return false;
}

bool  LibconfigWrapper::StoreBuildin(void *data, const char *name, LcwBaseType bt) 
{
	libconfig::Setting & root = GetPath();

	// bt value range is checked by CheckTypeAndModeAndStoreName()
	switch(bt) 
	{
	default:   // default case should never happen
	case T_BOOL:  root.add(name, libconfig::Setting::TypeBoolean) =         *(bool *)data;          return true; 
	case T_CHAR:  root.add(name, libconfig::Setting::TypeInt)     = (int)   *(char *)data;          return true;
	case T_UCHAR: root.add(name, libconfig::Setting::TypeInt)     = (int)   *(unsigned char *)data; return true;
	case T_SHORT: root.add(name, libconfig::Setting::TypeInt)     = (int)   *(short *)data;         return true;
	case T_USHORT:root.add(name, libconfig::Setting::TypeInt)     = (int)   *(unsigned short *)data;return true;
	case T_INT:   root.add(name, libconfig::Setting::TypeInt)     =         *(int *)data;           return true;
	case T_UINT:  
		{
			unsigned int ui = *(unsigned int *)data;

			if (ui<0x800000)
				root.add(name, libconfig::Setting::TypeInt)  = (int)  ui;  
			else 
				root.add(name, libconfig::Setting::TypeInt64)= (long long)  ui;  
		}
		return true;
	case T_FLOAT: root.add(name, libconfig::Setting::TypeFloat)   =         *(float *)data;         return true;
	case T_DOUBLE:root.add(name, libconfig::Setting::TypeFloat)   =         *(double *)data;        return true;
	}
}



bool  LibconfigWrapper::StoreArray(void *data, const char *name, LcwBaseType bt, int arrayLen) 
{		
	libconfig::Setting & root = GetPath();

	// Special case UCHAR/CHAR array could be saved as "" String
	if (bt==T_CHAR || bt==T_UCHAR) {
		char *str = (char *)data;
		if ((int)strlen(str) < arrayLen) {
			root.add(name,libconfig::Setting::TypeString) = str;
			return true;
		}
	}

	// Otherwise add a list of values

	libconfig::Setting & arrList = root.add(name,libconfig::Setting::TypeList);
	for(int i=0; i<arrayLen; i++) {
		// bt value range is checked by CheckTypeAndModeAndStoreName()
		switch(bt) 
		{
		case T_BOOL: arrList.add(libconfig::Setting::TypeBoolean)  =        ((bool *) data)[i]; break;
		case T_CHAR:  arrList.add(libconfig::Setting::TypeInt)     = (int)  ((char *) data)[i]; break;
		case T_UCHAR: arrList.add(libconfig::Setting::TypeInt)     = (int)  ((unsigned char *) data)[i]; break;
		case T_SHORT: arrList.add(libconfig::Setting::TypeInt)     = (int)  ((short *)         data)[i]; break;
		case T_USHORT:arrList.add(libconfig::Setting::TypeInt)     = (int)  ((unsigned short *)data)[i]; break;
		case T_INT:   arrList.add(libconfig::Setting::TypeInt)     =        ((int  *) data)[i]; break;
		case T_UINT:  
			{
				unsigned int ui = ((unsigned int *) data)[i];
				if (ui<0x80000000)
					arrList.add(libconfig::Setting::TypeInt) = (int)ui;
				else
					arrList.add(libconfig::Setting::TypeInt64) = (long long)ui;
			}
			break;
		case T_FLOAT: arrList.add(libconfig::Setting::TypeFloat)   =        ((float *) data)[i]; break;
		case T_DOUBLE:arrList.add(libconfig::Setting::TypeFloat)   =        ((double *)data)[i]; break;
		}
	}
	return true;
}

bool  LibconfigWrapper::Store(void *data, const char *name, LcwBaseType bt, LcwBaseMode bm, int arrayLen)
{
	// Setup type and name for debugging output, catches unknown BaseType or BaseMode
	if (!CheckTypeAndModeAndStoreName(name, bt, bm) ) {
		return false;
	}

	// The approach of using the return variable 'result' was choosen to archive 100% coverage 
	// by the unit tests. If the switch inside the try{} block returns directly, then the 
	// closing bracket of the try block is never reached by the coverage test.
	bool result = false; 

	try {
		libconfig::Setting & root = GetPath();

		// bm value range is checked by CheckTypeAndModeAndStoreName()
		switch(bm) 
		{
		default:   // default case should never happen
		case M_BUILDIN:  result = StoreBuildin( data,name,bt);           break;
		case M_ARRAY:    result = StoreArray(   data,name,bt,arrayLen);  break;
#ifndef LCW_NO_FUNCTYPE
		case M_FUNCTYPE: result =  StoreFuncType(data,name);             break;
#endif
#ifndef LCW_NO_NARRAY
		case M_NARRAY: 
			{
				// bt value range is checked by CheckTypeAndModeAndStoreName()
				switch(bt) 
				{
				default: // Default: should never happen
				case T_BOOL:  result = StoreNArray<bool>          (root, data, name, false);  break;
				case T_CHAR:  result = StoreNArray<char>          (root, data, name, false);  break;
				case T_UCHAR: result = StoreNArray<unsigned char> (root, data, name, false);  break; 
				case T_SHORT: result = StoreNArray<short>         (root, data, name, false);  break; 
				case T_USHORT:result = StoreNArray<unsigned short>(root, data, name, false);  break; 
				case T_INT:   result = StoreNArray<int>           (root, data, name, false);  break; 
				case T_UINT:  result = StoreNArray<unsigned int>  (root, data, name, false);  break; 
				case T_FLOAT: result = StoreNArray<float>         (root, data, name, true);   break; 
				case T_DOUBLE:result = StoreNArray<double>        (root, data, name, true);   break;
				}
			}
#endif
		}
	}
	catch( const libconfig::SettingNameException &snex)
	{
		sprintf_s(_currErrMsg,"Invalid Name in '%s' (%s)\n",snex.getPath(),snex.what());
	}
	return result;
}


bool  LibconfigWrapper::GroupBeg(char *name) {	
	// Setup type and name for debugging output
	CheckTypeAndModeAndStoreName(name, T_BOOL, M_GROUP_BEG);

	// Store current root
	try {
		libconfig::Setting & root = GetPath();

		if (_pathIdx==9) {
			sprintf_s(_currErrMsg,"Nesting overflow, only 10 nested groups supported\n");
			return false;
		}

		if (strlen(name)>63 ) {
			sprintf_s(_currErrMsg,"Name too long, maximum of 64 chars\n");
			return false;
		}

		_pathIdx++;
		strcpy_s(_pathPart[_pathIdx], name);

		if (_isParsing) {
			libconfig::Setting & checkSet = GetPath(); // Will thrown an error if not present
		}
		else {
			root.add(name, libconfig::Setting::TypeGroup );
		}
		return true;
	}
	catch(const libconfig::SettingNotFoundException &nfex) {
		sprintf_s(_currErrMsg,"Not found (%s)",nfex.what());
	}
	catch(const libconfig::SettingNameException &snex) {
		sprintf_s(_currErrMsg,"Invalid name (%s)",snex.what());
	}
	return false;
}

bool  LibconfigWrapper::GroupEnd(char *name) {
	// Setup type and name for debugging output
	CheckTypeAndModeAndStoreName(name, T_BOOL, M_GROUP_END);
	
	if (_pathIdx<0) {
	  sprintf_s(_currErrMsg,"Current path already empty\n");
	  return false;
	}

	if (strcmp(_pathPart[_pathIdx],name) != 0) {
	  sprintf_s(_currErrMsg,"Current path part differs '%s'\n",name);
	  return false;
	}

	_pathPart[_pathIdx][0] = 0;
	_pathIdx--;
	return true;
}

libconfig::Setting & LibconfigWrapper::GetPath() {
	
	libconfig::Setting & root = _cfg.getRoot();
	switch(_pathIdx) {
		default: 
		case -1: return root;
		case  0: return root[ _pathPart[0] ];
		case  1: return root[ _pathPart[0] ][ _pathPart[1] ];
		case  2: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ];
		case  3: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ];
		case  4: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ][ _pathPart[4] ];
		case  5: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ][ _pathPart[4] ][ _pathPart[5] ];
		case  6: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ][ _pathPart[4] ][ _pathPart[5] ][ _pathPart[6] ];
		case  7: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ][ _pathPart[4] ][ _pathPart[5] ][ _pathPart[6] ][ _pathPart[7] ];
		case  8: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ][ _pathPart[4] ][ _pathPart[5] ][ _pathPart[6] ][ _pathPart[7] ][ _pathPart[8] ];
		case  9: return root[ _pathPart[0] ][ _pathPart[1] ][ _pathPart[2] ][ _pathPart[3] ][ _pathPart[4] ][ _pathPart[5] ][ _pathPart[6] ][ _pathPart[7] ][ _pathPart[8] ][ _pathPart[9] ];
	}
}