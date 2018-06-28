/**
* \file UtParamManager.cpp
* \brief Implementation of UtParamManager to test the ParamManager class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnitTest.h"

#include "../soar_support_lib/ParamManager.h"
#include "../soar_support_lib/NArray.h"
#include "../soar_support_lib/FuncType.h"
#include "../soar_support_lib/ParamManager/LibconfigWrapperEnums.h"
#include "../soar_support_lib/ParamManager/ParBase.h"


/**
 * \ingroup Tests
 * \brief UnitTest for ParamManager class
 */
class UtParamManager : public UnitTest {
public:
	/// \brief The constructor registers at the UnitTestManager
	UtParamManager() : UnitTest("ParamManager")
	{ }


	/**
	 * \ingroup Tests
	 * \brief Local testclass for UtParamManager containing all possible parameter types
	 */
	class UtPmTest {
	private:
		ParamManager _pm;			///< The manager for all parameters of this instance beeing tested
		ParamManager _pmBuildins;	///< The manager for buildin parameters
		ParamManager _pmConstArrays;///< The manager for const array parameters
		ParamManager _pmArrays;		///< The manager for array parameters
		ParamManager _pmStrings;	///< The manager for string parameters
		ParamManager _pmNArrays;	///< The manager for NArray parameters
		ParamManager _pmFuncTypes;	///< The manager for FuncType parameters
		ParamManager _pmOthers;		///< The manager for mixed parameters

		/**
		* \name Buildin primitive types
		* @{ 
		*/
		bool           _b;
		char           _c;
		unsigned char  _uc;
		short          _s;
		unsigned short _us;
		int            _i;
		unsigned int   _ui;
		float          _f;
		double         _d;

		///@} End of group started by \name

		/**
		* \name Constant arrays of primitive types
		* @{ 
		*/
		bool           _bConstArray[3];
		char           _cConstArray[4];
		unsigned char  _ucConstArray[5];
		short          _sConstArray[6];
		unsigned short _usConstArray[7];
		int            _iConstArray[8];
		unsigned int   _uiConstArray[9];
		float          _fConstArray[10];
		double         _dConstArray[11];

		///@} End of group started by \name

		/**
		* \name Dynamic allocated arrays of primitive types
		* @{ 
		*/
		bool           * _bArray;
		char           * _cArray;
		unsigned char  * _ucArray;
		short          * _sArray;
		unsigned short * _usArray;
		int            * _iArray;
		unsigned int   * _uiArray;
		float          * _fArray;
		double         * _dArray;

		///@} End of group started by \name

		/**
		* \name Special case string as char[] or uchar[]
		* @{ 
		*/
		char             _cString[24];
		unsigned char    _ucString[26];
		char           * _cStringPtr;
		unsigned char  * _ucStringPtr;

		///@} End of group started by \name


		/**
		* \name NArrays with variing dimensions of primitive types
		* @{ 
		*/
		NArray<bool>			_bNArray;
		NArray<char>			_cNArray;
		NArray<unsigned char>   _ucNArray;
		NArray<int>				_iNArray;
		NArray<unsigned int>	_uiNArray;
		NArray<short>			_sNArray;
		NArray<unsigned short>  _usNArray;
		NArray<float>			_fNArray;
		NArray<double>			_dNArray;

		///@} End of group started by \name

		/**
		* \name FuncTypes with variing types
		* @{ 
		*/
		FuncType                _ftZero;	
		FuncType                _ftOne;	
		FuncType                _ftConst;	
		FuncType                _ftLin;	
		FuncType                _ftQuad;	
		FuncType                _ftHyp;	
		FuncType                _ftSig;	
		FuncType                _ftExp;	
		FuncType                _ftDy;	
		///@} End of group started by \name

		UnitTest                *_ut;		///< Pointer to access the random values of the UnitTestManager
		int                     _idx;	    ///< Index for the pseudo random numbers of the UnitTestManager

		/**
		* \name Fill parameter with pseudo random value
		* There are different functions for all supported primitiv types
		* @{ 
		*/
		void SetRandom(bool           &b)  { b  = (bool)(_ut->GetNextIntTestValue(_idx,0,10) > 5); }
		void SetRandom(char           &c)  { c  = (char)(' '+ _ut->GetNextIntTestValue(_idx,0,63)); }
		void SetRandom(unsigned char  &uc) { uc = (unsigned char)(' '+ _ut->GetNextIntTestValue(_idx,0,63)); }
		void SetRandom(short          &s)  { s  = (short)         _ut->GetNextIntTestValue(_idx,-32768,32767); }
		void SetRandom(unsigned short &us) { us = (unsigned short)_ut->GetNextIntTestValue(_idx,0,65535); }
		void SetRandom(int            &i)  { i  =                 _ut->GetNextIntTestValue(_idx,-100000,100000); }
		void SetRandom(unsigned int   &ui) { ui = (unsigned int)  _ut->GetNextIntTestValue(_idx,0,5000000); }
		void SetRandom(float          &f)  { f  = (float) _ut->GetNextTestValue(_idx); }
		void SetRandom(double         &d)  { d  =         _ut->GetNextTestValue(_idx); }

		///@} End of group started by \name

		/// \brief Allocates the dynamic arrays used in the test 
		void AllocArrays(){
			_bArray  = new bool[11];
			_cArray  = new char[10];
			_ucArray = new unsigned char[9];
			_sArray  = new short[8];
			_usArray = new unsigned short[7];
			_iArray  = new int[6];
			_uiArray = new unsigned int[5];
			_fArray  = new float[4];
			_dArray  = new double[3];

			_cStringPtr  = new char[28];
			_ucStringPtr = new unsigned char[30];

			memset(_cString,0,24);
			memset(_ucString,0,26);
			memset(_cStringPtr,0,28);
			memset(_ucStringPtr,0,30);
		}

		/// \brief Initializes all parameters to pseudo random values
		void InitRandomValues() {
			SetRandom(_b);
			SetRandom(_c);
			SetRandom(_uc);
			SetRandom(_s);
			SetRandom(_us);
			SetRandom(_i);                 
			SetRandom(_ui);
			SetRandom(_f);
			SetRandom(_d);  

			int i;
			for(i=0; i<3; i++)	SetRandom(_bConstArray[i]);
			for(i=0; i<4; i++)	SetRandom(_cConstArray[i]);
			for(i=0; i<5; i++)	SetRandom(_ucConstArray[i]);
			for(i=0; i<6; i++)	SetRandom(_sConstArray[i]);
			for(i=0; i<7; i++)	SetRandom(_usConstArray[i]);
			for(i=0; i<8; i++)	SetRandom(_iConstArray[i]);
			for(i=0; i<9; i++)	SetRandom(_uiConstArray[i]);
			for(i=0; i<10; i++)	SetRandom(_fConstArray[i]);
			for(i=0; i<11; i++)	SetRandom(_dConstArray[i]);
			
			for(i=0; i<11; i++)	SetRandom(_bArray[i]);
			for(i=0; i<10; i++)	SetRandom(_cArray[i]);
			for(i=0; i<9; i++)	SetRandom(_ucArray[i]);
			for(i=0; i<8; i++)	SetRandom(_sArray[i]);
			for(i=0; i<7; i++)	SetRandom(_usArray[i]);
			for(i=0; i<6; i++)	SetRandom(_iArray[i]);
			for(i=0; i<5; i++)	SetRandom(_uiArray[i]);
			for(i=0; i<4; i++)	SetRandom(_fArray[i]);
			for(i=0; i<3; i++)	SetRandom(_dArray[i]);


			sprintf_s(_cString,sizeof(_cString),"This is test #%d",_idx);
			sprintf_s((char *)_ucString,sizeof(_ucString),"Hello World #%d",_idx);
			sprintf_s(_cStringPtr,28,"28Chars idx=%d",_idx);
			sprintf_s((char *)_ucStringPtr,30,"Max30Chars idx=%d",_idx);

			_bNArray.Init(5);
			_cNArray = _cString;	// Initialized with values
			_ucNArray.Init(7);
			_sNArray.Init(9);
			_usNArray.Init(3,4);
			_iNArray.Init(2,3,4);
			_uiNArray.Init(2,2,2,2);
			_fNArray.Init(11);
			_dNArray.Init(3);

			for(unsigned int i=0; i<_bNArray.GetDim(0); i++)
				SetRandom( _bNArray[i] );

			for(unsigned int i=0; i<_ucNArray.GetDim(0); i++)
				SetRandom( _ucNArray[i] );

			for(unsigned int i=0; i<_sNArray.GetDim(0); i++)
				SetRandom( _sNArray(i) );
			
			for(unsigned int x=0; x<_usNArray.GetDim(0); x++) 
				for(unsigned int y=0; y<_usNArray.GetDim(1); y++)
					SetRandom( _usNArray(x,y) );
			
			for(unsigned int x=0; x<_iNArray.GetDim(0); x++)
				for(unsigned int y=0; y<_iNArray.GetDim(1); y++)
					for(unsigned int z=0; z<_iNArray.GetDim(2); z++)
						SetRandom( _iNArray(x,y,z) );

			for(unsigned int x=0; x<_uiNArray.GetDim(0); x++)
				for(unsigned int y=0; y<_uiNArray.GetDim(1); y++)
					for(unsigned int z=0; z<_uiNArray.GetDim(2); z++)
						for(unsigned int u=0; u<_uiNArray.GetDim(3); u++)
							SetRandom( _uiNArray(x,y,z,u) );

			for(unsigned int i=0; i<_fNArray.GetDim(0); i++)
				SetRandom( _fNArray(i) );
			
			for(unsigned int i=0; i<_dNArray.GetDim(0); i++)
				SetRandom( _dNArray(i) );


			double a,b,c;

			SetRandom(a);
			SetRandom(b);
			SetRandom(c);

			_ftZero.SetConstant(0);	
			_ftOne.SetConstant(1);	

			_ftConst.SetConstant(a);	
			_ftLin.SetLinear(a,b);
			_ftQuad.SetQuadratic(a,b,c);	
			_ftHyp.SetHyperbolic(a,b);
			_ftSig.SetSigmoid(a,b);
			_ftExp.SetExponential(a,b);
			_ftDy.SetDy(a,b);	
		}

		/// \brief Initializes the all parameter manager with the list of parameters
		void InitParamManager() {
			_pm.GroupBeg("Grp_BuildinTypes");
			_pm.Add(_b ,"Bool");
			_pm.Add(_c ,"Char");
			_pm.Add(_uc,"UChar");
			_pm.Add(_s ,"Short");
			_pm.Add(_us,"UShort");
			_pm.Add(_i ,"Int");
			_pm.Add(_ui,"UInt");
			_pm.Add(_f ,"Float");
			_pm.Add(_d ,"Double");
			_pm.GroupEnd("Grp_BuildinTypes");

			_pm.GroupBeg("Grp_ConstArrayOfBuildin");
			_pm.Add(_bConstArray ,"BoolConstArray");
			_pm.Add(_cConstArray ,"CharConstArray");
			_pm.Add(_ucConstArray,"UCharConstArray");
			_pm.Add(_sConstArray ,"ShortConstArray");
			_pm.Add(_usConstArray,"UShortConstArray");
			_pm.Add(_iConstArray ,"IntConstArray");
			_pm.Add(_uiConstArray,"UIntConstArray");
			_pm.Add(_fConstArray ,"FloatConstArray");
			_pm.Add(_dConstArray ,"DoubleConstArray");
			_pm.GroupEnd("Grp_ConstArrayOfBuildin");
			
			_pm.GroupBeg("Grp_DynArrayOfBuildin");
			_pm.Add(_bArray ,11,"BoolArray");
			_pm.Add(_cArray ,10,"CharArray");
			_pm.Add(_ucArray, 9,"UCharArray");
			_pm.Add(_sArray , 8,"ShortArray");
			_pm.Add(_usArray, 7,"UShortArray");
			_pm.Add(_iArray , 6,"IntArray");
			_pm.Add(_uiArray, 5,"UIntArray");
			_pm.Add(_fArray , 4,"FloatArray");
			_pm.Add(_dArray , 3,"DoubleArray");
			_pm.GroupEnd("Grp_DynArrayOfBuildin");


			_pm.GroupBeg("Grp_CharUCharStrings");
			_pm.Add(_cString,"CString");
			_pm.Add(_ucString,"UCString");
			_pm.Add(_cStringPtr, 28,"CStringPtr");
			_pm.Add(_ucStringPtr,30,"UCStringPtr");
			_pm.GroupEnd("Grp_CharUCharStrings");
			
			_pm.GroupBeg("Grp_NArrayOfBuildin");
			_pm.Add(_bNArray,"NArrayBool");
			_pm.Add(_cNArray,"NArrayChar");
			_pm.Add(_ucNArray,"NArrayUChar");
			_pm.Add(_sNArray,"NArrayShort");
			_pm.Add(_usNArray,"NArrayUShort");
			_pm.Add(_iNArray,"NArrayInt");
			_pm.Add(_uiNArray,"NArrayUInt");
			_pm.Add(_dNArray,"NArrayDouble");
			_pm.Add(_fNArray,"NArrayFloat");
			_pm.GroupEnd("Grp_NArrayOfBuildin");
			
			_pm.GroupBeg("Grp_FuncType");
			_pm.Add(_ftZero, "FuncType_Zero");
			_pm.Add(_ftOne, "FuncType_One");
			_pm.Add(_ftConst, "FuncType_Const");
			_pm.Add(_ftLin, "FuncType_Lin");
			_pm.Add(_ftQuad, "FuncType_Quad");
			_pm.Add(_ftHyp, "FuncType_Hyp");
			_pm.Add(_ftSig, "FuncType_Sig");
			_pm.Add(_ftExp, "FuncType_Exp");
			_pm.Add(_ftDy, "FuncType_Dy");
			_pm.GroupEnd("Grp_FuncType");
		}

		/**
		 * \brief Initializes the various parameter managers with the list of parameters
		 * These contain only parts of the configuration
		 */
		void InitOtherParamManagers() {
			_pmBuildins.Add(_b ,"Bool");
			_pmBuildins.Add(_c ,"Char");
			_pmBuildins.Add(_uc,"UChar");
			_pmBuildins.Add(_s ,"Short");
			_pmBuildins.Add(_us,"UShort");
			_pmBuildins.Add(_i ,"Int");
			_pmBuildins.Add(_ui,"UInt");
			_pmBuildins.Add(_f ,"Float");
			_pmBuildins.Add(_d ,"Double");

			_pmConstArrays.Add(_bConstArray ,"BoolConstArray");
			_pmConstArrays.Add(_cConstArray ,"CharConstArray");
			_pmConstArrays.Add(_ucConstArray,"UCharConstArray");
			_pmConstArrays.Add(_sConstArray ,"ShortConstArray");
			_pmConstArrays.Add(_usConstArray,"UShortConstArray");
			_pmConstArrays.Add(_iConstArray ,"IntConstArray");
			_pmConstArrays.Add(_uiConstArray,"UIntConstArray");
			_pmConstArrays.Add(_fConstArray ,"FloatConstArray");
			_pmConstArrays.Add(_dConstArray ,"DoubleConstArray");
			
			_pmArrays.Add(_bArray ,11,"BoolArray");
			_pmArrays.Add(_cArray ,10,"CharArray");
			_pmArrays.Add(_ucArray, 9,"UCharArray");
			_pmArrays.Add(_sArray , 8,"ShortArray");
			_pmArrays.Add(_usArray, 7,"UShortArray");
			_pmArrays.Add(_iArray , 6,"IntArray");
			_pmArrays.Add(_uiArray, 5,"UIntArray");
			_pmArrays.Add(_fArray , 4,"FloatArray");
			_pmArrays.Add(_dArray , 3,"DoubleArray");

			_pmStrings.Add(_cString,"CString");
			_pmStrings.Add(_ucString,"UCString");
			_pmStrings.Add(_cStringPtr, 28,"CStringPtr");
			_pmStrings.Add(_ucStringPtr,30,"UCStringPtr");
			
			_pmNArrays.Add(_bNArray,"NArrayBool");
			_pmNArrays.Add(_cNArray,"NArrayChar");
			_pmNArrays.Add(_ucNArray,"NArrayUChar");
			_pmNArrays.Add(_sNArray,"NArrayShort");
			_pmNArrays.Add(_usNArray,"NArrayUShort");
			_pmNArrays.Add(_iNArray,"NArrayInt");
			_pmNArrays.Add(_uiNArray,"NArrayUInt");
			_pmNArrays.Add(_dNArray,"NArrayDouble");
			_pmNArrays.Add(_fNArray,"NArrayFloat");
			
			_pmFuncTypes.Add(_ftZero, "FuncType_Zero");
			_pmFuncTypes.Add(_ftOne, "FuncType_One");
			_pmFuncTypes.Add(_ftConst, "FuncType_Const");
			_pmFuncTypes.Add(_ftLin, "FuncType_Lin");
			_pmFuncTypes.Add(_ftQuad, "FuncType_Quad");
			_pmFuncTypes.Add(_ftHyp, "FuncType_Hyp");
			_pmFuncTypes.Add(_ftSig, "FuncType_Sig");
			_pmFuncTypes.Add(_ftExp, "FuncType_Exp");
			_pmFuncTypes.Add(_ftDy, "FuncType_Dy");

			// The 'others' parameters manager contains mixed params with maximum of groups
			_pmOthers.GroupBeg("Group0");
			_pmOthers.Add(_b ,"Bool");
			_pmOthers.GroupBeg("Group1");
			_pmOthers.Add(_s ,"Short");
			_pmOthers.GroupBeg("Group2");
			_pmOthers.Add(_cConstArray ,"CharConstArray");
			_pmOthers.GroupBeg("Group3");
			_pmOthers.Add(_cString,"CString");
			_pmOthers.GroupBeg("Group4");
			_pmOthers.Add(_sNArray,"NArrayShort");
			_pmOthers.GroupBeg("Group5");
			_pmOthers.Add(_uiArray, 5,"UIntArray");
			_pmOthers.GroupBeg("Group6");
			_pmOthers.Add(_ucStringPtr,30,"UCStringPtr");
			_pmOthers.GroupBeg("Group7");
			_pmOthers.Add(_ftLin, "FuncType_Lin");
			_pmOthers.GroupBeg("Group8");
			_pmOthers.Add(_dNArray,"NArrayDouble");
			_pmOthers.GroupBeg("Group9");
			_pmOthers.Add(_ui,"UInt");
			_pmOthers.GroupEnd("Group9");
			_pmOthers.Add(_ftDy, "FuncType_Dy");
			_pmOthers.GroupEnd("Group8");
			_pmOthers.Add(_fConstArray ,"FloatConstArray");
			_pmOthers.GroupEnd("Group7");
			_pmOthers.Add(_us,"UShort");
			_pmOthers.GroupEnd("Group6");
			_pmOthers.Add(_cNArray,"NArrayChar");
			_pmOthers.GroupEnd("Group5");
			_pmOthers.Add(_d ,"Double");
			_pmOthers.GroupEnd("Group4");
			_pmOthers.Add(_ftExp, "FuncType_Exp");
			_pmOthers.GroupEnd("Group3");
			_pmOthers.Add(_iConstArray ,"IntConstArray");
			_pmOthers.GroupEnd("Group2");
			_pmOthers.Add(_uc,"UChar");
			_pmOthers.GroupEnd("Group1");
			_pmOthers.Add(_ftConst, "FuncType_Const");
			_pmOthers.GroupEnd("Group0");
		}

		bool nearlyEqual(double a, double b) {
			double absA = fabs(a);
			double absB = fabs(b);
			double diff = fabs(a - b);

			if (a == b) { // shortcut, handles infinities
				return true;
			} else if (a == 0 || b == 0 || diff < 0.00000001) {
				return true;
			} 
			// use relative error
			return diff / (absA + absB) < 0.0000001;
		}


		bool CheckFuncTypes(FuncType &a, FuncType &b) {
			if (a.GetType() != b.GetType() )
				return false;

			return nearlyEqual(a.GetParam(0), b.GetParam(0) ) 
				&& nearlyEqual(a.GetParam(1), b.GetParam(1) ) 
				&& nearlyEqual(a.GetParam(2), b.GetParam(2) );
		}

	public:
		/**
		 * \brief Constructor
		 * \param ut   Pointer to UtParamManager unit test to access the random values of UnitTestManager
		 * \param idx   Index for the pseudo random values managed by UnitTestManager
		 */
		UtPmTest(UnitTest *ut, int &idx) {
			AllocArrays();

			_idx = idx;
			_ut  = ut;
			InitRandomValues();
			idx = _idx;

			InitParamManager();
			InitOtherParamManagers();
		}

		bool CheckSame(UtPmTest &other)
		{
			// Buildins arrays
			if ( _b != other._b) {
				printf("UtPmTest: _b differs\n"); return false;
			}
			if ( _b != other._b) {
				printf("UtPmTest: _b differs\n"); return false;
			}
			if ( _c != other._c) {
				printf("UtPmTest: _c differs\n");  return false;
			}
			if ( _uc != other._uc) {
				printf("UtPmTest: _uc differs\n");  return false;
			}
			if ( _s != other._s) {
				printf("UtPmTest: _s differs\n");  return false;
			}
			if ( _us != other._us) {
				printf("UtPmTest: _us differs\n");  return false;
			}
			if ( _i != other._i) {
				printf("UtPmTest: _i differs\n");  return false;
			}
			if ( _ui != other._ui) {
				printf("UtPmTest: _ui differs\n");  return false;
			}
			if ( fabs(_f - other._f) >0.0000001) {
				printf("UtPmTest: _f differs\n");  return false;
			}
			if ( fabs(_d - other._d) >0.0000001) {
				printf("UtPmTest: _d differs\n");  return false;
			}


			// Const arrays
			for(int i=0; i<3; i++) {
				if (_bConstArray[i] != other._bConstArray[i]) {
					printf("UtPmTest: _bConstArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<4; i++) {
				if (_cConstArray[i] != other._cConstArray[i]) {
					printf("UtPmTest: _cConstArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<5; i++) {
				if (_ucConstArray[i] != other._ucConstArray[i]) {
					printf("UtPmTest: _ucConstArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<6; i++) {
				if (_sConstArray[i] != other._sConstArray[i]) {
					printf("UtPmTest: _sConstArray[%d] differs\n",i); return false;
				}
			}
			for(int i=0; i<7; i++) {
				if (_usConstArray[i] != other._usConstArray[i]) {
					printf("UtPmTest: _usConstArray[%d] differs\n",i); return false;
				}
			}
			for(int i=0; i<8; i++) {
				if (_iConstArray[i] != other._iConstArray[i]) {
					printf("UtPmTest: _iConstArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<9; i++) {
				if (_uiConstArray[i] != other._uiConstArray[i]) {
					printf("UtPmTest: _uiConstArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<10; i++) {
				if ( fabs(_fConstArray[i] - other._fConstArray[i])>0.000001) {
					printf("UtPmTest: _fConstArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<11; i++) {
				if ( fabs(_dConstArray[i] - other._dConstArray[i])>0.000001) {
					printf("UtPmTest: _dConstArray[%d] differs\n",i);  return false;
				}
			}

			// Dynamic arrays
			for(int i=0; i<11; i++) {
				if (_bArray[i] != other._bArray[i]) {
					printf("UtPmTest: _bArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<10; i++) {
				if (_cArray[i] != other._cArray[i]) {
					printf("UtPmTest: _cArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<9; i++) {
				if (_ucArray[i] != other._ucArray[i]) {
					printf("UtPmTest: _ucArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<8; i++) {
				if (_sArray[i] != other._sArray[i]) {
					printf("UtPmTest: _sArray[%d] differs\n",i); return false;
				}
			}
			for(int i=0; i<7; i++) {
				if (_usArray[i] != other._usArray[i]) {
					printf("UtPmTest: _usArray[%d] differs\n",i); return false;
				}
			}
			for(int i=0; i<6; i++) {
				if (_iArray[i] != other._iArray[i]) {
					printf("UtPmTest: _iArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<5; i++) {
				if (_uiArray[i] != other._uiArray[i]) {
					printf("UtPmTest: _uiArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<4; i++) {
				if ( fabs(_fArray[i] - other._fArray[i])>0.000001) {
					printf("UtPmTest: _fArray[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<3; i++) {
				if ( fabs(_dArray[i] - other._dArray[i])>0.000001) {
					printf("UtPmTest: _dArray[%d] differs\n",i);  return false;
				}
			}

			// Strings
			for(int i=0; i<24; i++) {
				if (_cString[i] != other._cString[i]) {
					printf("UtPmTest: _cString[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<26; i++) {
				if (_ucString[i] != other._ucString[i]) {
					printf("UtPmTest: _ucString[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<28; i++) {
				if (_cStringPtr[i] != other._cStringPtr[i]) {
					printf("UtPmTest: _cStringPtr[%d] differs\n",i);  return false;
				}
			}
			for(int i=0; i<30; i++) {
				if (_ucStringPtr[i] != other._ucStringPtr[i]) {
					printf("UtPmTest: _ucStringPtr[%d] differs\n",i);  return false;
				}
			}

			// NArrays
			if ( !(_bNArray==other._bNArray)) {
					printf("UtPmTest: _bNArray differs\n");  return false;
			}
			if ( !(_cNArray==other._cNArray)) {
					printf("UtPmTest: _cNArray differs\n");  return false;
			}
			if ( !(_ucNArray==other._ucNArray)) {
					printf("UtPmTest: _ucNArray differs\n");  return false;
			}
			if ( !(_sNArray==other._sNArray)) {
					printf("UtPmTest: _sNArray differs\n");  return false;
			}
			if ( !(_usNArray==other._usNArray)) {
					printf("UtPmTest: _usNArray differs\n");  return false;
			}
			if ( !(_iNArray==other._iNArray)) {
					printf("UtPmTest: _iNArray differs\n");  return false;
			}
			if ( !(_uiNArray==other._uiNArray)) {
					printf("UtPmTest: _uiNArray differs\n");  return false;
			}
			// Float/double binary comparison could fail...
			if (_fNArray.GetSize()==other._fNArray.GetSize() ) {
				for(unsigned int i=0; i<_fNArray.GetSize(); i++) {
					if (fabs(_fNArray.GetData()[i] - other._fNArray.GetData()[i]) > 0.0000001) {
						printf("UtPmTest: _fNArray[%d] differs\n",i);  return false;				
					}
				}
			}
			else {
				printf("UtPmTest: _fNArray differs in size\n");  return false;				
			}

			if (_dNArray.GetSize()==other._dNArray.GetSize() ) {
				for(unsigned int i=0; i<_dNArray.GetSize(); i++) {
					if (fabs(_dNArray.GetData()[i] - other._dNArray.GetData()[i]) > 0.0000001) {
						printf("UtPmTest: _dNArray[%d] differs\n",i);  return false;				
					}
				}
			}
			else {
				printf("UtPmTest: _dNArray differs in size\n");  return false;				
			}

			// FuncTypes
			if ( !CheckFuncTypes(_ftZero,other._ftZero) ) {
					printf("UtPmTest: _ftZero differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftOne,other._ftOne) ) {
					printf("UtPmTest: _ftOne differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftConst,other._ftConst) ) {
					printf("UtPmTest: _ftConst differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftLin,other._ftLin) ) {
					printf("UtPmTest: _ftLin differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftQuad,other._ftQuad) ) {
					printf("UtPmTest: _ftQuad differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftHyp,other._ftHyp) ) {
					printf("UtPmTest: _ftHyp differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftSig,other._ftSig) ) {
					printf("UtPmTest: _ftSig differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftExp,other._ftExp) ) {
					printf("UtPmTest: _ftExp differs\n");  return false;
			}
			if ( !CheckFuncTypes(_ftDy,other._ftDy) ) {
					printf("UtPmTest: _ftDy differs\n");  return false;
			}
			return true;
		}

		/// \brief Retrieves the main ParamManager of this instance
		ParamManager &GetPM()	{ return _pm; }	

		/// \brief Retrieves the ParamManager for buildin types
		ParamManager &GetPmBuildins()		{ return _pmBuildins; }

		/// \brief Retrieves the ParamManager for const array types
		ParamManager &GetPmConstArrays()	{ return _pmConstArrays; }

		/// \brief Retrieves the ParamManager for array types
		ParamManager &GetPmArrays()			{ return _pmArrays; }

		/// \brief Retrieves the ParamManager for string types
		ParamManager &GetPmStrings()		{ return _pmStrings; }

		/// \brief Retrieves the ParamManager for NArray types
		ParamManager &GetPmNArrays()		{ return _pmNArrays; }

		/// \brief Retrieves the ParamManager for FuncType types
		ParamManager &GetPmFuncTypes()		{ return _pmFuncTypes; }

		/// \brief Retrieves the ParamManager for mixed types
		ParamManager &GetPmOthers()			{ return _pmOthers; }
	};

	/// \brief Tests load/save/compare ascii and binary formats
	void TestFileIO() {
		TestGroup("Bin/Ascii - Load/Save/Compare");

		int idxA = 200, idxB = 300, idxC = 450;

		for(int tests=0; tests <10; tests++) {			
			printf("## Step 0:Build A,B and C with (different) random content\n");

			// Build instance A, B and C each with different random values
			UtPmTest A(this,idxA), B(this,idxB), C(this,idxC);

			// A saves binary and ascii ascii
			printf("## Step 1: A saves binary and ascii ascii\n");
			ExpectOkay( A.GetPM().SaveBinary("Ut_Pm_A.dat"), "A saving binary");
			ExpectOkay( A.GetPM().SaveAscii( "Ut_Pm_A.cfg"), "A saving ascii");


			// B compares binary from A, this should differ 
			printf("## Step 2: B compares binary from A, this should differ\n");
			ExpectFail( B.GetPM().CompareBinary("Ut_Pm_A.dat"), "B do not differ from A binary");
			ExpectFail( B.GetPM().CompareAscii( "Ut_Pm_A.cfg"), "B do not differ from A ascii");

			// C compares ascii from B, should differ			
			printf("## Step 3: C compares binary from A, this should differ\n");
			ExpectFail( C.GetPM().CompareBinary("Ut_Pm_A.dat"), "C do not differ from A binary");
			ExpectFail( C.GetPM().CompareAscii( "Ut_Pm_A.cfg"), "C do not differ from A ascii");

			// B loads binary from A, saves ascii	
			printf("## Step 4:  B loads binary from A, saves ascii\n");	
			ExpectOkay( B.GetPM().LoadBinary("Ut_Pm_A.dat"),     "B loading binary of A");
			ExpectOkay( B.GetPM().CompareBinary( "Ut_Pm_A.dat"), "B compare binary A");
			ExpectOkay( B.GetPM().SaveAscii( "Ut_Pm_B.cfg"),     "B saving ascii");

			// C loads ascii from B, saves binary
			printf("## Step 5:  C loads ascii from B, saves binary\n");
			ExpectOkay( C.GetPM().LoadAscii( "Ut_Pm_B.cfg"), "C loading ascii of B");
			ExpectOkay( C.GetPM().SaveBinary("Ut_Pm_C.dat"), "C saving binary");

			// C compares ascii from A
			printf("## Step 6:  C compares ascii from A\n");
			ExpectOkay( C.GetPM().CompareAscii(  "Ut_Pm_A.cfg"), "C comparing ascii of A");
			ExpectOkay( C.GetPM().WasLoadedFromFile("NArrayBool"), "C loaded NArrayBool");
			ExpectFail( C.GetPM().WasLoadedFromFile("NonExistant"), "C loaded NonExistant");

			// C compares values with A
			ExpectOkay( C.CheckSame( A ), "C comparing values with A");
			printf("## ------------------------------ \n\n");
		}
		TestGroup();
	}

		
	/// \brief Tests load/save/compare ascii and binary with 'bad' files
	void TestBadFileIO() {
		TestGroup("IO with bad files");
		{
			int idxA = 275;
			UtPmTest A(this,idxA);
			
			ExpectFail( A.GetPM().LoadBinary("FileDoesNotExist.dat"),   "LoadBinary() bad file");
			ExpectFail( A.GetPM().CompareBinary("FileDoesNotExist.dat"),"CompareBinary() bad file");
			ExpectFail( A.GetPM().SaveBinary(""),                       "SaveBinary() bad file");

			ExpectFail( A.GetPM().LoadAscii("FileDoesNotExist.cfg"),   "LoadAscii() bad file");
			ExpectFail( A.GetPM().CompareAscii("FileDoesNotExist.cfg"),"CompareAscii() asc bad file");
			ExpectFail( A.GetPM().SaveAscii(""),                       "SaveAscii() bad file");
		}
		TestGroup();
	}


	void TestMixedIoCheckPartial(ParamManager &pmAll, ParamManager &pm, char *testName, char *varName) {
		char datName[256],cfgName[56],info[256],*badParam;

		sprintf_s(datName,"Ut_Pm_B_%s.dat",testName);
		sprintf_s(cfgName,"Ut_Pm_B_%s.cfg",testName);

		sprintf_s(info,"A Reload, %s",testName);
		ExpectOkay( pmAll.LoadBinary("Ut_Pm_A_All.dat"),info);
		badParam = pmAll.GetNameOfFailedParam();
		ExpectOkay( strcmp(badParam,"<none>")==0, info);

		sprintf_s(info,"A CompareBinary() %s",testName);
		ExpectFail( pm.CompareBinary(datName), info);

		sprintf_s(info,"A CompareBinary() pre differs %s",testName);
		badParam = pm.GetNameOfFailedParam();
		ExpectOkay( strcmp(badParam,varName)==0, info);

		sprintf_s(info,"A CompareAscii() %s",testName);
		ExpectFail( pm.CompareAscii(cfgName), info);

		sprintf_s(info,"A CompareAscii() pre differs %s",testName);
		badParam = pm.GetNameOfFailedParam();
		ExpectOkay( strcmp(badParam,varName)==0, info);

		sprintf_s(info,"A LoadBinary() %s",testName);
		ExpectOkay( pm.LoadBinary(datName), info);

		sprintf_s(info,"A CompareBinary() mod %s",testName);
		ExpectFail( pmAll.CompareBinary("Ut_Pm_A_All.dat"), info);

		sprintf_s(info,"A CompareBinary() differs %s",testName);
		badParam = pmAll.GetNameOfFailedParam();
		ExpectOkay( strcmp(badParam,varName)==0, info);
	}

	/// \brief Tests load/save/compare ascii and binary with mixed files
	void TestMixedIO() {

		TestGroup("Mixed IO");
		{
			int idxA = 375, idxB = 413;
			UtPmTest A(this,idxA), B(this, idxB);

			ExpectOkay( A.GetPM().SaveBinary("Ut_Pm_A_All.dat"),    "A SaveBinary()");
			ExpectOkay( A.GetPM().SaveAscii( "Ut_Pm_A_All.cfg"),    "A SaveAscii()");

			ExpectOkay( B.GetPmBuildins().SaveBinary(   "Ut_Pm_B_Buildin.dat"),  "B SaveBinary() Buildins");
			ExpectOkay( B.GetPmStrings().SaveBinary(    "Ut_Pm_B_String.dat"),   "B SaveBinary() Strings");
			ExpectOkay( B.GetPmConstArrays().SaveBinary("Ut_Pm_B_CArray.dat"),   "B SaveBinary() ConstArrays");
			ExpectOkay( B.GetPmArrays().SaveBinary(     "Ut_Pm_B_Array.dat"),    "B SaveBinary() Arrays");
			ExpectOkay( B.GetPmNArrays().SaveBinary(    "Ut_Pm_B_NArray.dat"),   "B SaveBinary() NArrays");
			ExpectOkay( B.GetPmFuncTypes().SaveBinary(  "Ut_Pm_B_FuncType.dat"), "B SaveBinary() FuncTypes");
			ExpectOkay( B.GetPmOthers().SaveBinary(     "Ut_Pm_B_Others.dat"),   "B SaveBinary() Others");

			ExpectOkay( B.GetPmBuildins().SaveAscii(   "Ut_Pm_B_Buildin.cfg"),  "B SaveAscii() Buildins");
			ExpectOkay( B.GetPmStrings().SaveAscii(    "Ut_Pm_B_String.cfg"),   "B SaveAscii() Strings");
			ExpectOkay( B.GetPmConstArrays().SaveAscii("Ut_Pm_B_CArray.cfg"),   "B SaveAscii() ConstArrays");
			ExpectOkay( B.GetPmArrays().SaveAscii(     "Ut_Pm_B_Array.cfg"),    "B SaveAscii() Arrays");
			ExpectOkay( B.GetPmNArrays().SaveAscii(    "Ut_Pm_B_NArray.cfg"),   "B SaveAscii() NArrays");
			ExpectOkay( B.GetPmFuncTypes().SaveAscii(  "Ut_Pm_B_FuncType.cfg"), "B SaveAscii() FuncTypes");
			ExpectOkay( B.GetPmOthers().SaveAscii(     "Ut_Pm_B_Others.cfg"),   "B SaveAscii() Others");
			
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmBuildins(),    "Buildin",   "Bool");
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmStrings(),     "String",    "CString");
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmConstArrays(), "CArray",    "BoolConstArray");
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmArrays(),      "Array",     "BoolArray");
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmNArrays(),     "NArray",    "NArrayBool");
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmFuncTypes(),   "FuncType",  "FuncType_Const");
			TestMixedIoCheckPartial(A.GetPM(), A.GetPmOthers(),      "Others",    "Bool");

			ExpectFail( A.GetPmArrays().LoadBinary("Ut_Pm_B_Buildin.dat"),    "A.arr LoadBinary() too short data"); 
			ExpectFail( A.GetPmArrays().CompareBinary("Ut_Pm_B_Buildin.dat"), "A.arr CompareBinary() too short data"); 
			
			ExpectFail( A.GetPmArrays().LoadAscii("Ut_Pm_B_Buildin.cfg"),    "A.arr LoadAscii() wrong cfg"); 
			ExpectFail( A.GetPmArrays().CompareAscii("Ut_Pm_B_Buildin.cfg"), "A.arr CompareAscii() wrong cfg"); 
			ExpectFail( A.GetPmBuildins().CompareAscii("Ut_Pm_B_Array.cfg"), "A.build CompareAscii() wrong cfg"); 
			ExpectFail( A.GetPmNArrays().CompareAscii("Ut_Pm_B_Buildin.cfg"),"A.nArr CompareAscii() wrong cfg"); 
			ExpectFail( A.GetPmFuncTypes().CompareAscii("Ut_Pm_B_Buildin.cfg"),"A.ft CompareAscii() wrong cfg"); 
			ExpectFail( A.GetPmOthers().CompareAscii("Ut_Pm_B_Buildin.cfg"),"A.ot CompareAscii() wrong cfg"); 
		}

		// Failed ascii saving
		{			
			ParamManager pm;
			int i;

			pm.GroupBeg("Group");
			pm.Add(i,"I");
			pm.GroupEnd("Other"); // Saving this will fail

			ExpectFail( pm.SaveAscii("Ut_Pm_GroupErr.cfg"),"SaveAscii() with Grouping error"); 
		}

		// NArray special cases
		{
			ParamManager pm;

			NArray<float> arr(1,2,3);
			arr.Fill(42);
			pm.Add(arr,"NArray");

			ExpectOkay( pm.SaveAscii("Ut_Pm_NArray.cfg"),"SaveAscii() of NArray"); 
		}
		{
			ParamManager pm;

			NArray<float> arr(1,2);
			pm.Add(arr,"NArray");

			ExpectFail( pm.CompareAscii("Ut_Pm_NArray.cfg"),"CompareAscii() of NArray with differing dims"); 
		}
		{
			ParamManager pm;

			NArray<float> arr(1,2,4);
			pm.Add(arr,"NArray");

			ExpectFail( pm.CompareAscii("Ut_Pm_NArray.cfg"),"CompareAscii() of NArray with differing dim width"); 
		}
		{
			ParamManager pm;

			NArray<float> arr(1,2,3);
			arr.Fill(42);

			arr(0,1,2) = 41; // Last element differs
			pm.Add(arr,"NArray");

			ExpectFail( pm.CompareAscii("Ut_Pm_NArray.cfg"),"CompareAscii() of NArray with differing value"); 
		}

		TestGroup();
	}

	/// \brief Tests to check binary load/save/compare fails of ParPrim/ParArray
	void TestParamBase() {

		TestGroup("ParamBase");
		{
			FILE *file;

			// Just an empty file
			fopen_s(&file,"Ut_Pb.dat","wb");
			fclose(file);
		}
		{
			int data = 10;
			ParamManager pm;

			pm.Add(data,"Data");
			ExpectFail( pm.CompareBinary("Ut_Pb.dat"), "Binary compare ParPrim with empty file");
		}

		{
			int data[2];
			ParamManager pm;

			pm.Add(data,"Data");
			ExpectFail( pm.CompareBinary("Ut_Pb.dat"), "Binary compare ParArray with empty file");
			ExpectFail( pm.LoadBinary("Ut_Pb.dat"), "Binary load ParArray with empty file");
		}

		{
			FILE *file;

			// Just a file with array size, but array is missing
			fopen_s(&file,"Ut_Pb.dat","wb");
			int data=2;
			fwrite(&data,sizeof(data),1,file);
			fclose(file);
		}
		{
			int data[2];
			ParamManager pm;

			pm.Add(data,"Data");
			ExpectFail( pm.CompareBinary("Ut_Pb.dat"), "Binary compare ParArray with part file");
			ExpectFail( pm.LoadBinary("Ut_Pb.dat"), "Binary load ParArray with üart file");
		}
		{
			ParamManager::ParPrim pp(NULL,(LcwBaseType)1000,"Dummy",false, sizeof(int)); 

			ExpectOkay(strcmp(pp.GetTypeName(),"<Unsupported Type>")==0,"ParPrim with Invalid LcwBaseType");
		}
		{
			char arr[32];

			ParamManager::ParArray pa(arr,T_CHAR,M_ARRAY,"Dummy",false,sizeof(arr),sizeof(char) );

			FILE *file;
			// Open file for reading, but try 'writing' instead
			fopen_s(&file,"Ut_Pb.dat","rb");
			
			ExpectFail(pa.SaveBinary(file), "ParArray saveBinary on read-only file");
			fclose(file);
		}

		{
			FILE *file;
			char arr[32];


			ParamManager pm;
			pm.Add(arr,"Dummy");

			// Open file for reading, but try 'writing' instead
			fopen_s(&file,"Ut_Pb.dat","rb");
			ExpectFail(pm.SaveBinary(file), "ParamManager saveBinary on read-only file");
			fclose(file);
		}
		TestGroup();
	}

	void TestEmptyManager() {
		TestGroup("IO with empty parameter lists");

		ParamManager pm;

		ExpectFail( pm.LoadAscii(   "Ut_Pm_A.cfg"), "ParamManager LoadAscii");
		ExpectFail( pm.CompareAscii("Ut_Pm_A.cfg"), "ParamManager CompareAscii");
		ExpectFail( pm.SaveAscii(   "Ut_Pm_A.cfg"),  "ParamManager SaveAscii");

		ExpectFail( pm.LoadBinary(   "Ut_Pm_A.dat"), "ParamManager LoadBinary");
		ExpectFail( pm.CompareBinary("Ut_Pm_A.dat"), "ParamManager CompareBinary");
		ExpectFail( pm.SaveBinary(   "Ut_Pm_A.dat"), "ParamManager SaveBinary");
		
		FILE *file;
		fopen_s(&file,"Ut_Pm_A.dat","rb");
		ExpectFail( pm.LoadBinary(   file), "ParamManager LoadBinary file");
		ExpectFail( pm.CompareBinary(file), "ParamManager CompareBinary file");
		ExpectFail( pm.SaveBinary(   file), "ParamManager SaveBinary file");
		fclose(file);

		TestGroup();
	}

	/// \brief The entry point to start the tests of this UnitTest class
	void RunTests() {
		TestFileIO();
		TestBadFileIO();
		TestMixedIO();
		TestParamBase();
		TestEmptyManager();
	}
};

UtParamManager test_ParamManager;  ///< Global instance automatically registers to UnitTestManager
