/**
* \file ParamManager.cpp
* \brief Implementation of class ParamManager
*
* (C) Karsten Isakovic, Berlin 2016/2017 ( Karsten.Isakovic@web.de )
*/

#include "..\ParamManager.h"
#include <stdlib.h>

#include "ParBase.h"
#include "LibconfigWrapper.h"

ParamManager::ParamManager() : _head(0), _last(0), _curr(0)
{
}

ParamManager::~ParamManager()
{
	// Need to free all ParBa(se data structures, the Param<T> and ParamT<T> don't add members
	ParBase *curr,*next;

	curr = _head;
	while( curr != 0) {
		next = curr->_next;
		delete curr;
		curr = next;
	}
	_head = 0;
	_last = 0;
	_curr = 0;
}

void ParamManager::Link(ParBase *pb) 
{
	if ( !_head ) {
		_head = pb;
		_last = pb;
	}
	else {
		_last->_next = pb;
		_last        = pb;
	}
}

char *ParamManager::GetNameOfFailedParam()
{
	if (_curr==NULL)
		return "<none>";
	return _curr->_name;
}


void ParamManager::Add(bool &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_BOOL, name,optional,sizeof(bool)) );
}

void ParamManager::Add(char &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_CHAR, name,optional,sizeof(char)) );
}

void ParamManager::Add(unsigned char &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_UCHAR, name,optional,sizeof(unsigned char)) );
}

void ParamManager::Add(short &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_SHORT, name,optional,sizeof(short)) );
}

void ParamManager::Add(unsigned short &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_USHORT, name,optional,sizeof(unsigned short)) );
}

void ParamManager::Add(int &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_INT, name,optional,sizeof(int)) );
}

void ParamManager::Add(unsigned int &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_UINT, name,optional,sizeof(unsigned int)) );
}

void ParamManager::Add(float &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_FLOAT, name,optional,sizeof(float)) );
}

void ParamManager::Add(double &addr, char *name, bool optional) {
	Link( new ParPrim(&addr,T_DOUBLE, name,optional,sizeof(double)) );
}


void ParamManager::Add(FuncType &addr, char *name, bool optional) {
	Link( new ParClass<FuncType>(&addr,T_DOUBLE, M_FUNCTYPE,name,optional) );
}


/**
 * \cond
 * if not suppressed by the cond , these functions would list as public 
 * functions of ParamManager, even though they are only specializations 
 * of the template void Add(NArray<T>     &addr...) and AddArray<T>(&addr...)
 * functions defined in the header.
 */
template <> void ParamManager::Add(NArray<bool> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<bool> >(&addr,T_BOOL, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<char> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<char> >(&addr,T_CHAR, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<unsigned char> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<unsigned char> >(&addr,T_UCHAR, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<short> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<short> >(&addr,T_SHORT, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<unsigned short> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<unsigned short> >(&addr,T_USHORT, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<int> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<int> >(&addr,T_INT, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<unsigned int> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<unsigned int> >(&addr,T_UINT, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<float> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<float> >(&addr,T_FLOAT, M_NARRAY,name,optional) );
}
template <> void ParamManager::Add(NArray<double> &addr, char *name, bool optional) {
	Link( new ParClass< NArray<double> >(&addr,T_DOUBLE, M_NARRAY,name,optional) );
}


template <> void ParamManager::AddArray<bool>(const bool * addr, char *name, bool optional, unsigned int N) {
	Link( new ParArray( (void *)addr,T_BOOL, M_ARRAY,name,optional,N,sizeof(bool) ) );
}

template <> void ParamManager::AddArray<char>(const char * addr, char *name, bool optional, unsigned int N) {
	Link( new ParArray( (void *)addr,T_CHAR, M_ARRAY,name,optional,N,sizeof(char) ) );
}

template <> void ParamManager::AddArray<unsigned char>(const unsigned char * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_UCHAR, M_ARRAY,name,optional,N,sizeof(unsigned char) ) );
}

template <> void ParamManager::AddArray<short>(const short * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_SHORT, M_ARRAY,name,optional,N,sizeof(short) ) );
}

template <> void ParamManager::AddArray<unsigned short>(const unsigned short * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_USHORT, M_ARRAY,name,optional,N,sizeof(unsigned short) ) );
}

template <> void ParamManager::AddArray<int>(const int * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_INT, M_ARRAY,name,optional,N,sizeof(int) ) );
}

template <> void ParamManager::AddArray<unsigned int>(const unsigned int * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_UINT, M_ARRAY,name,optional,N,sizeof(unsigned int) ) );
}

template <> void ParamManager::AddArray<float>(const float * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_FLOAT, M_ARRAY,name,optional,N,sizeof(float) ) );
}

template <> void ParamManager::AddArray<double>(const double * addr, char *name, bool optional, unsigned int N) {	
	Link( new ParArray( (void *)addr,T_DOUBLE, M_ARRAY,name,optional,N,sizeof(double) ) );
}
/// \endcond


void ParamManager::GroupBeg(char *cfg_group) {
	Link( new ParGroup(M_GROUP_BEG,cfg_group) );
}

void ParamManager::GroupEnd(char *cfg_group) {
	Link( new ParGroup(M_GROUP_END,cfg_group) );
}

bool ParamManager::SaveBinary(char *name)
{
	FILE *file = 0;
	fopen_s(&file,name,"wb");
	if (file==0) {
		printf("ParamManager::SaveBinary(%s) error opening the file for writing\n",name);
		return false;
	}
	bool ret = SaveBinary(file);
	fclose(file);	
	return ret;
}

bool ParamManager::LoadBinary(char *name)
{	
	FILE *file = 0;
	fopen_s(&file,name,"rb");
	if (file==0) {
		printf("ParamManager::LoadBinary(%s) error opening the file for reading\n",name);
		return false;
	}
	bool ret = LoadBinary(file);
	fclose(file);	
	return ret;
}

bool ParamManager::CompareBinary(char *name)
{
	FILE *file = 0;
	fopen_s(&file,name,"rb");
	if (file==0) {
		printf("ParamManager::CompareBinary(%s) error opening the file for reading\n",name);
		return false;
	}
	bool ret = CompareBinary(file);
	fclose(file);	
	return ret;
}

bool ParamManager::SaveBinary(FILE *f)
{
	if (!_head) {
		printf("ParamManager::SaveBinary() parameter list is empty\n");
		return false;
	}

	_curr = _head;
	while(_curr!=NULL) {
//		fprintf(stderr,"SB %08x %s\n",_curr->_data, _curr->_name);
		if (!_curr->SaveBinary(f))
			return false;
		_curr = _curr->_next;
	}
	_curr = 0;
	return true;
}

bool ParamManager::LoadBinary(FILE *f)
{	
	if (!_head) {
		printf("ParamManager::LoadBinary() parameter list is empty\n");
		return false;
	}

	// Set all config reads (With binary format all optional data is loaded)
	_curr = _head;
	while(_curr!=NULL) {
		_curr->_readFromFile = true;
		_curr = _curr->_next;
	}

	_curr = _head;
	while(_curr!=NULL) {
//		fprintf(stderr,"LB %08x %s\n",_curr->_data, _curr->_name);
		if (!_curr->LoadBinary(f) ) 
			return false;
		_curr = _curr->_next;
	}
	_curr = NULL;
	return true;
}

bool ParamManager::CompareBinary(FILE *f)
{
	if (!_head) {
		printf("ParamManager::CompareBinary() parameter list is empty\n");
		return false;
	}

	_curr = _head;
	while(_curr!=NULL) {
//		fprintf(stderr,"CB %08x %s\n",_curr->_data, _curr->_name);
		if (!_curr->CompareBinary(f) )
			return false;

		_curr = _curr->_next;
	}
	_curr = NULL;
	return true;
}

bool ParamManager::SaveAscii(char *configName)
{
//	printf("SaveAscii '%s'\n",configName);
	if (!_head) {
		printf("ParamManager::SaveAscii(%s) parameter list is empty\n",configName);
		return false;
	}

	// Try to load the config file
	LibconfigWrapper lcw;

	// Step through all parameters and try to save them
	bool saveOkay = true;

	_curr = _head;
	while(_curr!=NULL) {

		bool result;
		switch(_curr->_mode) {
			case M_BUILDIN: 
			case M_FUNCTYPE:
			case M_NARRAY: 	
				result = lcw.Store(_curr->_data, _curr->_name, _curr->_type, _curr->_mode);
				break;
			case M_ARRAY:
				result = lcw.Store(_curr->_data, _curr->_name, _curr->_type, _curr->_mode, ((ParArray *)_curr)->_maxLen);
				break;

			case M_GROUP_BEG:
				result = lcw.GroupBeg( _curr->_name );
				break;
			case M_GROUP_END:
				result = lcw.GroupEnd( _curr->_name );
				break;
		}

		if ( !result ) {
			printf("%s\n",lcw.GetLastParamErrorMsg() );		
			saveOkay = false;
		}
		_curr = _curr->_next;
	}
	_curr = NULL;		

	if (!lcw.Save(configName) ) {
		printf("%s\n",lcw.GetLastParamErrorMsg() );
		saveOkay = false;
	}

	if (!saveOkay)
		printf("ParamManager::SaveAscii(%s) config had errors\n\n",configName);

	return saveOkay;
}


bool ParamManager::LoadAscii(char *configName)
{
//	printf("LoadAscii '%s'\n",configName);
	if (!_head) {
		printf("ParamManager::LoadAscii(%s) parameter list is empty\n",configName);
		return false;
	}

	// Clear all config reads
	_curr = _head;
	while(_curr!=NULL) {
		_curr->_readFromFile = false;
		_curr = _curr->_next;
	}

	// Try to load the config file
	LibconfigWrapper lcw;
	if (!lcw.Load(configName) ) {
		printf("%s\n",lcw.GetLastParamErrorMsg() );
		return false;
	}

	// Step through all parameters and try to load them
	bool loadOkay = true;
	_curr = _head;
	while(_curr!=NULL) {

		bool result;
		switch(_curr->_mode) {
			case M_BUILDIN: 
			case M_FUNCTYPE:
			case M_NARRAY: 			
				result = lcw.Parse(_curr->_data, _curr->_name, _curr->_type, _curr->_mode);
				break;
			case M_ARRAY:
				result = lcw.Parse(_curr->_data, _curr->_name, _curr->_type, _curr->_mode, ((ParArray *)_curr)->_maxLen);
				break;

			case M_GROUP_BEG:
				result = lcw.GroupBeg( _curr->_name );
				break;
			case M_GROUP_END:
				result = lcw.GroupEnd( _curr->_name );
				break;
		}

		if ( !result ) {
			
			if (lcw.GetLastParamFoundInCfgFile() || !_curr->_opt) {
				printf("%s\n",lcw.GetLastParamErrorMsg() );

				loadOkay = false;
			}
		}
		else {
			_curr->_readFromFile = true;
		}

		_curr = _curr->_next;
	}
	_curr = NULL;		

	if (!loadOkay)
		printf("ParamManager::LoadAscii(%s) config had errors\n\n",configName);

	// Return true if all non-optional parameters could be loaded
	return loadOkay;
}


template<typename T> bool ParamManager::CompareAsciiBuildin(LibconfigWrapper *lcw, ParBase *curr)
{
	T valFile;

	bool result = lcw->Parse(&valFile, _curr->_name, _curr->_type, _curr->_mode);
	if (result) {
		T valMem = *(T *)_curr->_data;

		if (curr->_type==T_FLOAT || curr->_type==T_DOUBLE) {
			if ( fabs((double)valMem - (double)valFile) > 0.00000001) {
				printf("  %s %s differs (mem=%f  cfg=%f)\n",_curr->GetTypeName(), _curr->_name, valMem,valFile); 
				return false;
			}
		}
		else {
			if (valMem!=valFile) {
				printf("  %s %s differs (mem=%d  cfg=%d)\n",_curr->GetTypeName(), _curr->_name, valMem,valFile); 
				return false;
			}
		}
	}
	else {
		printf("%s\n",lcw->GetLastParamErrorMsg() );
	}
	return result;
}

template<typename T> bool ParamManager::CompareAsciiNArray(LibconfigWrapper *lcw, ParBase *curr)
{
	NArray<T> arrFile;

	bool result = lcw->Parse(&arrFile, _curr->_name, _curr->_type, _curr->_mode);
	if (result) {
		NArray<T> arrMem = *(NArray<T> *)_curr->_data;

		int dimsFile = arrFile.GetDims();
		int dimsMem  = arrMem.GetDims();
		if ( dimsFile!=dimsMem  ) {
			printf("  NArray<%s> %s differs in dimensions (mem=%d D cfg=%d D)\n",_curr->GetTypeName(), _curr->_name, dimsFile,dimsMem); 
			return false;
		}

		for(int i=0; i<dimsMem; i++) {
			int dimFile = arrFile.GetDim(i);
			int dimMem  = arrMem.GetDim(i);
			if ( dimFile!=dimMem  ) {
				printf("  NArray<%s> %s differs in dimension %d widht (mem=%d cfg=%d)\n",_curr->GetTypeName(), _curr->_name, i, dimMem, dimFile); 
				return false;
			}
		}

		if (curr->_type==T_FLOAT || curr->_type==T_DOUBLE) {
			int size = arrMem.GetSize();
			for(int i=0; i<size; i++) {
				T valMem  = arrMem.GetData() [ i ];
				T valFile = arrFile.GetData() [ i ];
				if ( fabs((double) valMem- (double)valFile) > 0.00000001) {
					printf("  NArray<%s> %s differs in value (mem=%f  cfg=%f)\n",_curr->GetTypeName(), _curr->_name, valMem,valFile); 
					return false;
				}
			}			
			return true;
		}

		result = arrMem==arrFile;
		if (!result) {
			printf("  NArray<%s> %s differs in value\n",_curr->GetTypeName(), _curr->_name); 
		}
		return result;
	}

	printf("%s\n",lcw->GetLastParamErrorMsg() );
	return false;
}

template<typename T> bool ParamManager::CompareAsciiArray(LibconfigWrapper *lcw, ParBase *curr)
{
	ParArray *pc = (ParArray *)curr;

	T *valFile = new T[pc->_maxLen];

	bool result = lcw->Parse(valFile, _curr->_name, _curr->_type, _curr->_mode,pc->_maxLen);
	if (result) {
		T *valMem = (T *)_curr->_data;

		for(int i=0; i<pc->_maxLen; i++) {
			if (curr->_type==T_FLOAT || curr->_type==T_DOUBLE) {
				if ( fabs((double)valMem[i] - (double)valFile[i])>0.001) {
					printf("  %s %s[%d] differs (mem=%f  cfg=%f)\n",_curr->GetTypeName(), _curr->_name, i, valMem[i],valFile[i]); 
					return false;
				}
			}
			else {
				if (valMem[i]!=valFile[i]) {
					printf("  %s %s[%d] differs (mem=%d  cfg=%d)\n",_curr->GetTypeName(), _curr->_name, i, valMem[i],valFile[i]); 
					return false;
				}		
			}
		}
	}
	else {
		printf("%s\n",lcw->GetLastParamErrorMsg() );
	}
	return result;
}


bool ParamManager::CompareAsciiFuncType(LibconfigWrapper *lcw, ParBase *curr )
{
	FuncType ftFile;

	bool result = lcw->Parse(&ftFile, curr->_name, curr->_type, curr->_mode);
	if (result) {
		FuncType ftMem = *(FuncType *)curr->_data;

		if (   ftMem.GetType()!=ftFile.GetType()
			|| fabs(ftMem.GetParam(0) - ftFile.GetParam(0) ) > 0.00000001
			|| fabs(ftMem.GetParam(1) - ftFile.GetParam(1) ) > 0.00000001
			|| fabs(ftMem.GetParam(2) - ftFile.GetParam(2) ) > 0.00000001 ) {
				printf("  FUNCTYPE %s differs (mem=%s  cfg=%s)\n", curr->_name, ftMem.ToString("x"),ftFile.ToString("x")); 
				return false;
		}
		return true;
	}

	printf("%s\n",lcw->GetLastParamErrorMsg() );
	return false;
}

template<typename T> bool ParamManager::CompareAsciiType(LibconfigWrapper *lcw, ParBase *curr) {
	if (_curr->_mode==M_ARRAY) {
		return CompareAsciiArray<T>(  lcw, _curr);
	}
	else if (_curr->_mode==M_NARRAY) {
		return CompareAsciiNArray<T>( lcw, _curr);
	}	
	return CompareAsciiBuildin<T>( lcw, _curr);
}

bool ParamManager::CompareAscii(char *configName)
{
	//printf("CompareAscii '%s'\n",configName);
	if (!_head) {
		printf("ParamManager::CompareAscii(%s) parameter list is empty\n",configName);
		return false;
	}

	// Try to load the config file
	LibconfigWrapper lcw;
	if (!lcw.Load(configName) ) {
		printf("%s\n",lcw.GetLastParamErrorMsg() );
		return false;
	}

	// Step through all parameters and try to load them
	bool loadOkay = true;
	_curr = _head;

	ParBase *firstFailed = NULL;

	while(_curr!=NULL) {

		bool result;
		bool compResult = true;

		switch(_curr->_mode) 
		{
		case M_BUILDIN:
		case M_NARRAY:	
		case M_ARRAY:   
			switch(_curr->_type) 
			{
			default: // Default case should never happen
			case T_BOOL:  result = CompareAsciiType<bool>          (&lcw, _curr); break;
			case T_CHAR:  result = CompareAsciiType<char>          (&lcw, _curr); break; 
			case T_UCHAR: result = CompareAsciiType<unsigned char> (&lcw, _curr); break;  
			case T_SHORT: result = CompareAsciiType<short>         (&lcw, _curr); break; 
			case T_USHORT:result = CompareAsciiType<unsigned short>(&lcw, _curr); break;  
			case T_INT:   result = CompareAsciiType<int>           (&lcw, _curr); break; 
			case T_UINT:  result = CompareAsciiType<unsigned int>  (&lcw, _curr); break;
			case T_FLOAT: result = CompareAsciiType<float>         (&lcw, _curr); break; 
			case T_DOUBLE:result = CompareAsciiType<double>        (&lcw, _curr); break; 
			}
			break;

		case M_FUNCTYPE: result = CompareAsciiFuncType(&lcw, _curr); break;
		case M_GROUP_BEG:
			result = lcw.GroupBeg( _curr->_name );
			if (!result) {
				printf("%s\n",lcw.GetLastParamErrorMsg() );
			}	
			break;
		case M_GROUP_END:
			result = lcw.GroupEnd( _curr->_name );
			if (!result) {
				printf("%s\n",lcw.GetLastParamErrorMsg() );
			}	
			break;
		}

		if ( !result ) {
			loadOkay = false;
			if (firstFailed==NULL) {
				firstFailed = _curr;
			}
		}

		_curr = _curr->_next;
	}
	_curr = firstFailed;		

	if (!loadOkay)
		printf("CompareAscii(%s) config had errors\n\n",configName);

	// Return true if all non-optional parameters could be loaded
	return loadOkay;
}


bool ParamManager::WasLoadedFromFile(char *paramName)
{	
	_curr = _head;
	while(_curr!=NULL) {
		if (!strcmp(_curr->_name, paramName) )
			return _curr->_readFromFile;

		_curr = _curr->_next;
	}
	_curr = NULL;	

	// Error if parameter not found at all.
	return false;
}
