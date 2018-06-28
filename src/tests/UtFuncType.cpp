/**
* \file UtFuncType.cpp
* \brief Implementation of UtFuncType to test the FuncType class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/
#include <math.h>
#include <stdlib.h>

#include "UnitTest.h"

#include "../soar_support_lib/FuncType.h"


/**
 * \ingroup Tests
 * \brief UnitTest for FuncType class
 */
class UtFuncType : public UnitTest {
private:
	double _eps;				///< Epsilon used for cpmparison
	int    _funcs;              ///< Number of function parameters to try
	int    _checks;             ///< Number of checks per parameterised function
public:

	/// \brief The constructor registers at the UnitTestManager
	UtFuncType() : UnitTest("FuncType") , _eps(0.0000001), _funcs(25), _checks(50)
	{
	}


	/// \brief Tests FuncType.Constant() with random numbers
	void TestConstant() {
		FuncType ft;

		TestGroup("FuncType.Constant()");
		bool okay = true;
		int idx0 = 0;

		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0,2);

			ft.SetConstant(a0);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,2);
				double returnVal = ft( x );
				double expectVal = a0; 

				okay = fabs( returnVal - expectVal) < _eps;
				ExpectOkay(okay, "Result of Constant(%f)(%f) returned %f instead of %f",a0,x,returnVal,expectVal );

				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}

	/// \brief Tests FuncType.Linear() with random numbers
	void TestLinear() {
		FuncType ft;

		TestGroup("FuncType.Linear()");
		bool okay = true;
		int idx0 = 0;
		int idx1 = 1;
		
		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0,1);;
			double a1 = GetNextTestValue(idx1,2);;

			ft.SetLinear(a0,a1);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,3);
				double returnVal = ft( x );
				double expectVal = a0 + a1*x; 

				okay = fabs( returnVal - expectVal) < _eps;
				ExpectOkay(okay, "Result of Linear(%f,%f)(%f) returned %f instead of %f",a0,a1,x,returnVal,expectVal );
				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}
	
	/// \brief Tests FuncType.Quadratic() with random numbers
	void TestQuadratic() {
		FuncType ft;

		TestGroup("FuncType.Quadratic()");
		int idx0 = 0;
		int idx1 = 1;
		int idx2 = 2;
		bool okay = true;

		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0);
			double a1 = GetNextTestValue(idx1, 2);
			double a2 = GetNextTestValue(idx2, 3);

			ft.SetQuadratic(a0,a1,a2);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,4);
				double returnVal = ft( x );
				double expectVal = a0 + a1*x + a2*x*x; 

				okay = fabs( returnVal - expectVal) < _eps;
				ExpectOkay(okay, "Result of Quadratic(%f,%f,%f)(%f) returned %f instead of %f",a0,a1,a2,x,returnVal,expectVal );

				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}

	/// \brief Tests FuncType.Hyperbolic() with random numbers
	void TestHyperbolic() {
		FuncType ft;

		TestGroup("FuncType.Hyperbolic()");
		int idx0 = 0;
		int idx1 = 1;
		bool okay = true;

		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0);
			double a1 = GetNextTestValue(idx1, 2);

			ft.SetHyperbolic(a0,a1);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,3);
				// prevent division by zero
				if (fabs(x+a1) > _eps) {
					double returnVal = ft( x );
					double expectVal = a0 * x / (x + a1);

					okay = fabs( returnVal - expectVal) < _eps;
					ExpectOkay(okay, "Result of Hyperbolic(%f,%f)(%f) returned %f instead of %f",a0,a1,x,returnVal,expectVal );
				}			
				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}

	/// \brief Tests FuncType.Sigmoid() with random numbers
	void TestSigmoid() {
		FuncType ft;

		TestGroup("FuncType.Sigmoid()");
		int idx0 = 0;
		int idx1 = 1;
		bool okay = true;

		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0);
			double a1 = GetNextTestValue(idx1, 2);

			ft.SetSigmoid(a0,a1);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,3);
				// prevent division by zero
				if (fabs(x*x+a1) > _eps) {
					double returnVal = ft( x );
					double expectVal = a0 * x*x / (x*x + a1);

					okay = fabs( returnVal - expectVal) < _eps;
					ExpectOkay(okay, "Result of Sigmoid(%f,%f)(%f) returned %f instead of %f",a0,a1,x,returnVal,expectVal );
				}
				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}

	/// \brief Tests FuncType.Exponential() with random numbers
	void TestExponential() {
		FuncType ft;

		TestGroup("FuncType.Exponential()");
		int idx0 = 0;
		int idx1 = 1;
		bool okay = true;

		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0);
			double a1 = GetNextTestValue(idx1, 2);

			ft.SetExponential(a0,a1);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,3);
				// prevent division by zero
				if (fabs(x*x+a1) > _eps) {
					double returnVal = ft( x );
					double expectVal = a0*x*exp(a1*x) / exp(a1); 

					// Ignore too large values
					if (fabs(expectVal) < 1e30) {
						okay = fabs( returnVal - expectVal) < _eps;
						ExpectOkay(okay, "Result of Exponential(%f,%f)(%f) returned %f instead of %f",a0,a1,x,returnVal,expectVal );
					}
				}
				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}

	/// \brief Tests FuncType.Dy() with random numbers
	void TestDy() {
		FuncType ft;

		TestGroup("FuncType.Dy()");
		int idx0 = 0;
		int idx1 = 1;
		bool okay = true;

		for(int f=0; f<_funcs; f++) {
			double a0 = GetNextTestValue(idx0);
			double a1 = GetNextTestValue(idx1, 2);

			ft.SetDy(a0,a1);

			for(int idxT=0, c=0; c<_checks; c++) {
				double x         = GetNextTestValue(idxT,3);
				// prevent division by zero
				if (fabs(a1) > _eps) {
					double returnVal = ft( x );
					double p= 1 - a1*x;
					double expectVal = a0 + (1-a0)* p*p*p*p*p*p*p*p; 

					// Ignore too large values
					if (fabs(expectVal) < 1e40) {
						double flexEps = _eps;
						if (fabs(expectVal) > 1e6)
							flexEps = _eps * fabs(expectVal);
						double delta = fabs( returnVal - expectVal);
						okay = delta < flexEps;

						ExpectOkay(okay, "Result of Dy(%f,%f)(%f) returned %f instead of %f  (delta %f, eps=%f)",a0,a1,x,returnVal,expectVal,delta,flexEps );
					}
				}
				if (!okay)
					break;
			}
			if (!okay)
				break;
		}
		TestGroup();
	}	

	bool SaveBinary(char *name, FuncType &ft)
	{
		FILE *file = 0;
		fopen_s(&file,name,"wb");
		if (file==0) {
			printf("UtFuncType::SaveBinary(%s) error opening the file for writing\n",name);
			return false;
		}
		bool ret = ft.SaveBinary(file);
		fclose(file);	
		return ret;
	}

	bool LoadBinary(char *name, FuncType &ft)
	{
		FILE *file = 0;
		fopen_s(&file,name,"rb");
		if (file==0) {
			printf("UtFuncType::LoadBinary(%s) error opening the file for reading\n",name);
			return false;
		}
		bool ret = ft.LoadBinary(file);
		fclose(file);	
		return ret;
	}
	bool CompareBinary(char *name, FuncType &ft)
	{
		FILE *file = 0;
		fopen_s(&file,name,"rb");
		if (file==0) {
			printf("UtFuncType::CompareBinary(%s) error opening the file for reading\n",name);
			return false;
		}
		bool ret = ft.CompareBinary(file);
		fclose(file);	
		return ret;
	}

	/// \brief Tests FuncType serialization
	void TestLoadSaveCompare() {	
		TestGroup("Load/Save/Compare");
		FuncType ftA,ftB,ftC;

		ftA.SetConstant(1);
		ftB.SetQuadratic(1,2,3);
		ftC.SetQuadratic(1,2,4);

		ExpectOkay( SaveBinary("Ut_Ft_A.dat",ftA), "SaveBinary A");
		ExpectOkay( SaveBinary("Ut_Ft_B.dat",ftB), "SaveBinary B");
		ExpectOkay( SaveBinary("Ut_Ft_C.dat",ftC), "SaveBinary C");
		
		ExpectFail( CompareBinary("Ut_Ft_A.dat",ftB), "ComapreBinary A with B");
		ExpectFail( CompareBinary("Ut_Ft_A.dat",ftC), "ComapreBinary A with C");

		ExpectFail( CompareBinary("Ut_Ft_B.dat",ftA), "ComapreBinary B with A");
		ExpectFail( CompareBinary("Ut_Ft_B.dat",ftC), "ComapreBinary B with C");

		ExpectFail( CompareBinary("Ut_Ft_C.dat",ftA), "ComapreBinary C with A");
		ExpectFail( CompareBinary("Ut_Ft_C.dat",ftB), "ComapreBinary C with B");

		ExpectOkay( LoadBinary("Ut_Ft_C.dat",ftA), "LoadBinary C to A");
		ExpectOkay( LoadBinary("Ut_Ft_C.dat",ftB), "LoadBinary C to B");
		
		ExpectOkay( CompareBinary("Ut_Ft_C.dat",ftA), "ComapreBinary C with A after load");
		ExpectOkay( CompareBinary("Ut_Ft_C.dat",ftB), "ComapreBinary C with B after load");

		TestGroup();
	}
	
	/// \brief Tests FuncType operators and other members
	void TestUtilities() {

		TestGroup("Utility functions");
		{
			FuncType ftA,ftB,ftC,ftD,ftE;

			ftB.SetConstant(0);
			ftC.SetConstant(1);
			ftD.SetConstant(2);

			printf("  A = %s\n",ftA.ToString("x") );
			printf("  B = %s\n",ftB.ToString("x") );
			printf("  C = %s\n",ftC.ToString("x") );
			printf("  D = %s\n",ftD.ToString("x") );
			ExpectOkay(ftA.GetType()==FuncType::FT_NONE, "FuncType none");
			ExpectOkay(ftB.GetType()==FuncType::FT_ZERO, "FuncType zero");
			ExpectOkay(ftC.GetType()==FuncType::FT_ONE, "FuncType one");
			ExpectOkay(ftD.GetType()==FuncType::FT_CONSTANT, "FuncType const");

			
			ExpectOkay(ftB.IsZero(), "FuncType B is zero");
			ExpectOkay(ftC.IsOne(), "FuncType C is one");
			ExpectFail(ftD.IsZero(), "FuncType D is zero");
			ExpectFail(ftD.IsOne(),  "FuncType D is one");

			ExpectOkay(ftE.GetType()==FuncType::FT_NONE, "FuncType E none");
			ftE = ftD;
			printf("  E = %s\n",ftE.ToString("x") );
			ExpectOkay(ftE.GetType()==FuncType::FT_CONSTANT, "FuncType E now constant");

			ExpectOkay(ftA(10)==0, "FuncType none always zero");
			ExpectOkay(ftB(10)==0, "FuncType zero always zero");
			ExpectOkay(ftC(10)==1, "FuncType one  always one");
		}

		{
			FuncType ftA,ftB;

			ftA.SetLinear(1,0); // Should produce warning
			ftB.SetLinear(1,2); // Should produce no warning
			printf("  A = %s\n",ftA.ToString("x") );
			printf("  B = %s\n",ftB.ToString("x") );
			
			ExpectOkay(ftA.GetType()==FuncType::FT_LINEAR, "FuncType linear");
			ExpectOkay(ftA.DidWarn(), "FuncType linA warning");
			ExpectFail(ftB.DidWarn(), "FuncType linB warning");
		}

		{
			FuncType ftA,ftB,ftC,ftD;

			ftA.SetQuadratic(3,0,0);	// Should produce warning
			ftB.SetQuadratic(4,1,0);	// Should produce warning
			ftC.SetQuadratic(2,3,4);	// Should produce no warning
			printf("  A = %s\n",ftA.ToString("x") );
			printf("  B = %s\n",ftB.ToString("x") );
			printf("  C = %s\n",ftC.ToString("x") );

			ExpectOkay(ftA.GetType()==FuncType::FT_QUADRATIC, "FuncType quadratic");
			ExpectOkay(ftA.DidWarn(), "FuncType quadA warning");
			ExpectOkay(ftB.DidWarn(), "FuncType quadB warning");
			ExpectFail(ftC.DidWarn(), "FuncType quadC warning");

			ftD = ftC;
			printf("  D = %s\n",ftD.ToString("x") );
			ExpectOkay(ftD.GetParam(0)==2, "FuncType quadD param a");
			ExpectOkay(ftD.GetParam(1)==3, "FuncType quadD param b");
			ExpectOkay(ftD.GetParam(2)==4, "FuncType quadD param c");
		}

		{
			FuncType ftA,ftB,ftC,ftD,ftE;

			ftA.SetHyperbolic(3,0);	// Should produce warning
			ftB.SetHyperbolic(0,1);	// Should produce warning
			ftC.SetHyperbolic(2,3);	// Should produce no warning
			printf("  A = %s\n",ftA.ToString("x") );
			printf("  B = %s\n",ftB.ToString("x") );
			printf("  C = %s\n",ftC.ToString("x") );

			ftD.DoNotWarn();
			ftE.DoNotWarn();
			ftD.SetHyperbolic(3,0);	// Should produce warning, but supressed
			ftE.SetHyperbolic(0,1);	// Should produce warning, but supressed
			printf("  D = %s\n",ftD.ToString("x") );
			printf("  E = %s\n",ftE.ToString("x") );

			ExpectOkay(ftA.GetType()==FuncType::FT_HYPERBOLIC, "FuncType hyperbolic");
			ExpectOkay(ftA.DidWarn(), "FuncType hypA warning");
			ExpectOkay(ftB.DidWarn(), "FuncType hypB warning");
			ExpectFail(ftC.DidWarn(), "FuncType hypC warning");
			ExpectFail(ftD.DidWarn(), "FuncType hypD warning");
			ExpectFail(ftE.DidWarn(), "FuncType hypE warning");
		}

		{
			FuncType ftA,ftB,ftC,ftD;

			ftA.SetSigmoid(1,2);
			ftB.SetExponential(2,3);
			ftC.SetDy(4,5);
			printf("  A = %s\n",ftA.ToString("x") );
			printf("  B = %s\n",ftB.ToString("x") );
			printf("  C = %s\n",ftC.ToString("x") );

			ExpectOkay(ftA.GetType()==FuncType::FT_SIGMOID, "FuncType sigmoid");
			ExpectOkay(ftB.GetType()==FuncType::FT_EXPONENTIAL, "FuncType exponential");
			ExpectOkay(ftC.GetType()==FuncType::FT_DY, "FuncType dy");
		}



		TestGroup();
	}

	void RunTests() {
		TestConstant();
		TestLinear();
		TestQuadratic();
		TestHyperbolic();
		TestSigmoid();
		TestExponential();
		TestDy();

		TestLoadSaveCompare();
		TestUtilities();
	}
};

UtFuncType test_FuncType;  ///< Global instance automatically registers to UnitTestManager
