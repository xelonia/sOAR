/**
* \file FuncType.h
* \brief Declaration and inline implementation of class FuncType
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#ifndef FUNCTYPE_H
#define FUNCTYPE_H

#include <Math.h>
#include <stdio.h>
#include <stdlib.h>


/**
* \ingroup SoarSupport
* \ingroup SoarLib
*
* \brief Support class for variable function types.
*
* <DIV class="groupHeader">Versions</DIV>
* \code
*   26.10.2018 Added comparison operator
*   16.01.2016 Added assignment operator, comments and getter functions
*   29.08.2015 Initial version
* \endcode
*
* <DIV class="groupHeader">Copyright</DIV>
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*
* <HR />
*
* <H2 class="groupheader">Include</H2>
*/
class FuncType {
public:	
	/// \brief Enumates the supported function types
	enum FuncTypeVariant {
		FT_NONE        = 0, ///< No function defined (initial state)
		FT_ZERO        = 1, ///< F=0.0
		FT_ONE         = 2, ///< F=1.0
		FT_CONSTANT    = 3, ///< F=a
		FT_LINEAR      = 4, ///< F=a + bx
		FT_QUADRATIC   = 5, ///< F=a + bx + cx^2
		FT_HYPERBOLIC  = 6, ///< F=ax / (x + _b)
		FT_SIGMOID     = 7, ///< F=ax^2 / (x^2 + b)
		FT_EXPONENTIAL = 8, ///< F=ax ( exp(_bx) / exp(b) )
		FT_DY          = 9  ///< F=a + (1-a) (1-bx)^8 
	};

private:
	double _a;					///< Parameter A
	double _b;					///< Parameter B
	double _c;					///< Parameter C
	FuncTypeVariant _type;		///< Type of function

	char _tmpStr[256];          ///< Storage for string returned by ToString()
	bool _warn;					///< Enables warnings during function setup
	bool _didWarn;				///< There was a warnings during function setup

	bool IsParamZero(double v) { return fabs(v)  <0.00000000001;}  ///< Checks if v=0
	bool IsParamOne(double v)  { return fabs(v-1)<0.00000000001;}  ///< Checks if v=1
public:

	/// \brief Constructor, initializes type to FT_NONE and parameters zero and enables warnings
	FuncType() {
		_type = FT_NONE;
		_a = _b = _c = 0;

		_warn    = true;
		_didWarn = false;
	}

	/**
	 * \name Function setup
	 * @{ 
	 */

	/// \brief Initializes instance to constant function F=a
	void SetConstant(double a) {
		_type = FT_CONSTANT;
		_a   = a;

		if ( IsParamZero(_a) )
			_type = FT_ZERO;
		else if ( IsParamOne(_a) )
			_type = FT_ONE;
	}

	/// \brief Initializes instance to linear function F=ax + b
	void SetLinear(double a, double b) {
		_type = FT_LINEAR;
		_a   = a;
		_b   = b;

		if (_warn && IsParamZero(b) ) {
			printf("WARN: FuncType.SetLinear(%g,%g)  equals  %s \nHINT: Use  SetConstant(%g)\n",a,b,ToString("X"),a);
			_didWarn = true;
		}
	}

	/// \brief Initializes instance to quadratic function F=ax^2 +bx +c
	void SetQuadratic(double a, double b, double c) {
		_type = FT_QUADRATIC;
		_a   = a;
		_b   = b;
		_c   = c;

		if (_warn && IsParamZero(b) && IsParamZero(c) ) {
			printf("WARN: FuncType.SetQuadratic(%g,%g,%g)  equals  %s \nHINT: Use  SetConstant(%g)\n",a,b,c,ToString("X"),a);
			_didWarn = true;
		}
		else if (_warn && IsParamZero(c) ) {
			printf("WARN: FuncType.SetQuadratic(%g,%g,%g)  equals  %s \nHINT: Use  SetLinear(%g,%g)\n",a,b,c,ToString("X"),a,b);
			_didWarn = true;
		}
	}

	/// \brief Initializes instance to hyperbolic function F=ax / (x + _b)
	void SetHyperbolic(double a, double b) {
		_type = FT_HYPERBOLIC;
		_a   = a;
		_b   = b;

		if (_warn && (IsParamZero(a) || IsParamZero(b)) ) {
			printf("WARN: FuncType.SetHyperbolic(%g,%g)  equals  %s \nHINT:   Use  SetConstant(%g) instead\n",a,b,ToString("X"),a);
			_didWarn = true;
		}
	}

	/// \brief Initializes instance to sigmoid function F=ax^2 / (x^2 + b)
	void SetSigmoid(double a, double b) {
		_type = FT_SIGMOID;
		_a   = a;
		_b   = b;
	}

	/// \brief Initializes instance to exponential function F=ax ( exp(_bx) / exp(b) )
	void SetExponential(double a, double b) {
		_type = FT_EXPONENTIAL;
		_a   = a;
		_b   = b;
	}

	/// \brief Initializes instance to dy function F=1/52a + (1 - 1/52a)(1-bx)^8
	void SetDy(double a, double b) {
		_type = FT_DY;
		_a   = a;
		_b   = b;
	}
	
	///@}


	/**
	 * \brief Evaluates the function
	 * \param val  Value for the function variable
	 * \return     Value of function
	 */
	double operator() (double val) {
		switch(_type) {
		case FT_ZERO        : return 0.0;
		case FT_ONE         : return 1.0;
		case FT_CONSTANT    : return _a;
		case FT_LINEAR      : return _a + _b * val;
		case FT_QUADRATIC   : return _a + _b * val + _c * val*val;
		case FT_HYPERBOLIC  : return _a * val/ (val + _b);
		case FT_SIGMOID     : return _a * val*val / (val*val + _b);
		case FT_EXPONENTIAL : return _a*val* exp(_b*val) / exp(_b);
		case FT_DY			: 
			{
				double p = 1- _b*val;

				double p2 = p*p;
				double p4 = p2*p2;
				double p8 = p4*p4;

				return( _a + (1-_a) * p8 );
			}
		default:
			return 0;
		}

	}

	/**
	 * \name Binary IO functions, can be called by ParamManager
	 * @{ 
	 */

	/// \brief Initialize instance from file.
	bool LoadBinary(FILE *file) {
		int size = 0;

		size += fread(&_a,  sizeof(double), 1, file);
		size += fread(&_b,  sizeof(double), 1, file);
		size += fread(&_c,  sizeof(double), 1, file);
		size += fread(&_type,sizeof(FuncTypeVariant), 1, file);
		return size==4;
	}

	/// \brief Save instance to file.
	bool SaveBinary(FILE *file) {
		int size = 0;

		size += fwrite(&_a,  sizeof(double), 1, file);
		size += fwrite(&_b,  sizeof(double), 1, file);
		size += fwrite(&_c,  sizeof(double), 1, file);
		size += fwrite(&_type,sizeof(FuncTypeVariant), 1, file);
		return size==4;
	}

	/// \brief Binary comparison of instance with file.	
	bool CompareBinary(FILE *file) {
		FuncType tmpFT;
		tmpFT.LoadBinary(file);
		
		return _type==tmpFT._type && _a==tmpFT._a && _b==tmpFT._b && _c==tmpFT._c; 
	}
	
	///@}


	/**
	 * \name Utility functions
	 * @{ 
	 */

	/// \brief Returns FuncTypeVariant of instance
	FuncTypeVariant GetType() 
	{ 
		return _type; 
	}

	/// \brief Returns parameter of instance
	double GetParam(int i)
	{
		switch(i) 
		{
		default:
		case 0:	return _a;
		case 1:	return _b;
		case 2:	return _c;
		}
	}

	/// \brief Disables warning
	void DoNotWarn() {
		_warn = false;
	}

	/// \brief retriefe if there was a warning during setup
	bool DidWarn() {
		return _didWarn;
	}

	/// \brief Assignment operator
	FuncType & operator= ( FuncType &ft ) {
		_a = ft._a;
		_b = ft._b;
		_c = ft._c;
		_type = ft._type;
		_warn = ft._warn;
		return *this;
	}	
	
	/// \brief Comparison operator. 
	bool operator==(const FuncType &other) const {
		return _type == other._type && _a==other._a && _b==other._b && _c==other._c; 
	}

	/// \brief Returns if function equals constant 0.0
	bool IsZero() {
		return _type==FT_ZERO;
	}

	/// \brief Returns if function equals constant 1.0
	bool IsOne() {
		return _type==FT_ONE;
	}
	
	/**
	 * \brief Convert function to string (for output)
	 * \param var    String used for variable name
	 * \return       A string describing the function
     */
	char *ToString(char *var) {	
		double eps = 0.000001;
		_tmpStr[0] = 0;

		switch(_type) {
		case FT_ZERO:
		case FT_ONE:
		case FT_CONSTANT: 
			if (_a<0)
				sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g)",_a);
			else
				sprintf_s(_tmpStr,sizeof(_tmpStr),"%g",_a);
			return _tmpStr;

		case FT_LINEAR: 
			sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g + %g*%s)",_a,_b,var);
			return _tmpStr;

		case FT_QUADRATIC:
			sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g + %g*%s + %g*%s*%s)",_a,_b,var,_c,var,var);
			return _tmpStr;


		case FT_HYPERBOLIC  :
			if (_a<0)
				sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g * %s) / (%g+ %s)",_a,var, _b,var);
			else				
				sprintf_s(_tmpStr,sizeof(_tmpStr),"%g * %s / (%g+ %s)",_a,var, _b,var);
			return _tmpStr;

		case FT_SIGMOID     : 
			if (_a<0)
				sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g * %s^2) / (%g^2+ %s^2)",_a,var, _b,var);
			else				
				sprintf_s(_tmpStr,sizeof(_tmpStr),"%g * %s^^2 / (%g^2+ %s^2)",_a,var, _b,var);
			return _tmpStr;

		case FT_EXPONENTIAL: 
			if (_a<0)
				sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g * %s) * exp(%g*%s) / exp(%g)",_a,var, _b,var,_b);
			else				
				sprintf_s(_tmpStr,sizeof(_tmpStr),"%g * %s * exp(%g*%s) / exp(%g)",_a,var, _b,var,_b);
			return _tmpStr;

		case FT_DY: 
			sprintf_s(_tmpStr,sizeof(_tmpStr),"(%g  + (1-%g) * (1-%g*%s)^8 )",_a,_a,_b,var);
			return _tmpStr;

		case FT_NONE:
		default:	
				sprintf_s(_tmpStr,sizeof(_tmpStr),"(ERROR funcType.ToString type=%d)",_type);
			return _tmpStr;
		}
	}
		
	///@}
};

#endif