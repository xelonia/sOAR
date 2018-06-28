/**
* \file UtOptimizer.cpp
* \brief Implementation of UtOptimizer to test the Optimizer class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#include "UnitTest.h"

#include "../soar_lib/Optimizer.h"


/**
 * \ingroup Tests
 * \brief UnitTest for Optimizer class
 */
class UtOptimizer : public UnitTest {
public:
	/// \brief The constructor registers at the UnitTestManager
	UtOptimizer() : UnitTest("Optimizer")
	{ }


	/// \brief Tests X^3 type functions with optimizer
	void TestX3() {

		/// \brief Local class for calling the Optimizer with a cublic function
		class UtX3OptimizerFunc : public Optimizer::OptimizerFunc {
		private:
			double _a,_b,_c,_d;
		public:
			UtX3OptimizerFunc(double a, double b, double c, double d) : _a(a), _b(b), _c(c), _d(d) 	{}
			double operator()(double x) { return _a*x*x*x + _b*x*x + _c*x + _d; }
		};


		Optimizer optimizer;

		TestGroup("X^3 type functions");
		int tests=0;
		int idx0 = 0;
		int idx1 = 1;
		int idx2 = 2;
		int idx3 = 3;
		bool okay = true;
		while(tests<1000) {
			double a = GetNextTestValue(idx0);
			double b = GetNextTestValue(idx1, 2);
			double c = GetNextTestValue(idx2, 3);
			double d = GetNextTestValue(idx3, 5);

			// Derivate test
			if (fabs(a)>0.000000001 && 4*b*b - 12*a*a*c >0) {
				// The function ax^3 + bx^2 + cx +d has a minimum and maximum
				
				UtX3OptimizerFunc utX3Func(a,b,c,d);
				
				double xMi = 0;
				double xMa = 0;
				
				// f'(x)=3a*x*x + 2*b*x + c
				// Wir suchen f'(x) = 0

				// <>   0 = x *x + 2b/3a * x + c/3a
				// pq Formel
				double p = (2*b) / (3*a);
				double q =     c / (3*a);

				double p2 = p/2;

				double sq = p2*p2 - q;
				if (sq>=0) {					
					tests++;

					double sqr = sqrt(sq);

					double x1 = -p2 + sqr;
					double x2 = -p2 - sqr;

					// Now check f" (x)=6ax + 2b

					if ((6*a*x1 + 2*b) > 0) {
						xMi = x1;
						xMa = x2;
					}
					else {
						xMi = x2;
						xMa = x1;
					}
				

					double iB = xMi - (xMa-xMi);	// Wenn xMi kleiner xMa
					double iE = xMa;

					if (xMa<xMi) {
						iB = xMa;
						iE = xMi + (xMi-xMa);
					}

					double minimumX = optimizer.Brent_fmin(iB,iE, utX3Func, 1.0e-10);

					double flexEps = 0.0001;
					if (fabs(xMi)>1000)
						flexEps = flexEps * fabs(xMi);

					bool result = fabs(minimumX - xMi) < flexEps;

					ExpectOkay(result, "Local minium %f differs from %f for (%f,%f,%f,%f)",minimumX, xMi,a,b,c,d );

					if (!result) {
						double v1 = a*x1*x1*x1 + b*x1*x1 + c*x1 + d;
						double v2 = a*x2*x2*x2 + b*x2*x2 + c*x2 + d;
						// Breakpoint here, if there are errors
						int stop=1;
					}
				}

			}
		}
		TestGroup();
	}

	/// \brief Tests X^3 with cos() wobble type functions with optimizer
	void TestX3Cos() {

		/// \brief Local class for calling the Optimizer with a cublic plus cos wobble function
		class UtX3CosOptimizerFunc : public Optimizer::OptimizerFunc {
		private:
			double _a,_b,_c,_d;
			double _cs,_cf,_co;
		public:
			UtX3CosOptimizerFunc(double a, double b, double c, double d, double cs, double cf,double co) 
				: _a(a), _b(b), _c(c), _d(d), _cs(cs), _cf(cf), _co(co)	{}
			double operator()(double x) { return _a*x*x*x + _b*x*x + _c*x + _d + _cs*cos(_cf*(x+_co)); }
		};


		Optimizer optimizer;

		TestGroup("X^3 + cos() wobble functions");
		int tests=0;
		int idx0 = 0;
		int idx1 = 1;
		int idx2 = 2;
		int idx3 = 3;
		bool okay = true;
		while(tests<1000) {
			double a = GetNextTestValue(idx0);
			double b = GetNextTestValue(idx1, 2);
			double c = GetNextTestValue(idx2, 3);
			double d = GetNextTestValue(idx3, 5);

			// Derivate test
			if (fabs(a)>0.000000001 && 4*b*b - 12*a*a*c >0) {
				// The function ax^3 + bx^2 + cx +d has a minimum and maximum				
				
				double xMi = 0;
				double xMa = 0;
				
				// f'(x)=3a*x*x + 2*b*x + c
				// Wir suchen f'(x) = 0

				// <>   0 = x *x + 2b/3a * x + c/3a
				// pq Formel
				double p = (2*b) / (3*a);
				double q =     c / (3*a);

				double p2 = p/2;

				double sq = p2*p2 - q;
				if (sq>=0) {	

					double sqr = sqrt(sq);

					double x1 = -p2 + sqr;
					double x2 = -p2 - sqr;

					// Now check f" (x)=6ax + 2b

					if ((6*a*x1 + 2*b) > 0) {
						xMi = x1;
						xMa = x2;
					}
					else {
						xMi = x2;
						xMa = x1;
					}


					double iB = xMi - (xMa-xMi);	// Wenn xMi kleiner xMa
					double iE = xMa;

					if (xMa<xMi) {
						iB = xMa;
						iE = xMi + (xMi-xMa);
					}

					double vMin = a*xMi*xMi*xMi + b*xMi*xMi + c*xMi + d;
					double vMax = a*xMa*xMa*xMa + b*xMa*xMa + c*xMa + d;

					double prop = fabs(vMax-vMin)/fabs(xMa-xMi);
					// Reject the function if difference is too small
					if (vMax - vMin > 0.1 && prop  > 0.1 && prop<10) {

						// Position the co() wobble in a way that cs * cos( cf*(x-co) ) results 
						// in cos(0) * cs with a negative, small cs at xMinimum so that the functions
						// minimum does not change
						double cs = -(vMax - vMin)/40.0;
						double cf = 3.14/(xMa-xMi)*30;
						double co = -xMi;


						tests++;
						UtX3CosOptimizerFunc utX3Func(a,b,c,d,cs,cf,co);

						double minimumX = optimizer.Brent_fmin(iB,iE, utX3Func, 1.0e-10);


						bool result = fabs(minimumX - xMi) < 0.001;

						ExpectOkay(result, "Local minium %f differs from %f for (%f,%f,%f,%f)",minimumX, xMi,a,b,c,d );

						if (!result) {
							// Breakpoint here, if there are errors
							int stop=1;
						}
					}
				}

			}
		}
		TestGroup();
	}


	/**
	 * \brief Tests 1D ackley function with optimizer
	 * See https://en.wikipedia.org/wiki/Test_functions_for_optimization
	 */
	void TestAckley() {

		/// \brief Local class for calling the Optimizer with a ackley test function
		class UtAckleyOptimizerFunc : public Optimizer::OptimizerFunc {
		private:
			double _a,_b,_c,_d,_e;
		public:
			UtAckleyOptimizerFunc(double a, double b, double c, double d, double e) 
				: _a(a), _b(b), _c(c), _d(d), _e(e)	{}
			double operator()(double x) { 
				return _a*exp( _b*sqrt( _c*x*x )) - exp( _d*(cos(2*M_PI*x) ))+M_E+_e; }
		};


		Optimizer optimizer;

		TestGroup("Ackley1D functions");
		int tests=0;
		int idx0 = 0;
		int idx1 = 1;
		int idx2 = 2;
		int idx3 = 3;
		int idx4 = 3;
		bool okay = true;
		while(tests<1000) {
			double a = GetNextTestValue(idx0);
			double b = GetNextTestValue(idx1, 2);
			double c = GetNextTestValue(idx2, 3);

		
			// With c=0 there is no dependancy on x
			if (fabs(a)>0.001 && fabs(a)<=10000 && fabs(b)>0.001 && fabs(c)>0.001) {

				// Adjust 'sign' of parameters
				if (a>0) a=-a;

				if (b>0) b=-b;

				if (c<0) c=-c;

				// Adjust parameter range
				while (fabs(b) > fabs(a)*0.1 )
					b *= 0.01;

				while (fabs(c) > fabs(a)*0.05 )
					c *= 0.01;

				double d = c;
				double e = -a;

				/*
				a = -20;
				b = -0.2;
				c = 0.5;
				d = 0.5;
				e = 20;
				*/

				double expectedX = 0;

				tests++;
				UtAckleyOptimizerFunc utX3AckleyFunc(a,b,c,d,e);

				double minimumX = optimizer.Brent_fmin(-5,5, utX3AckleyFunc, 1.0e-10);


				bool result = fabs(minimumX - expectedX) < 0.00001;

				ExpectOkay(result, "Result localMinium %f differs from 0",minimumX );

				if (!result) {
					// Breakpoint here, if there are errors
					int stop=1;
				}
			}
		}
		TestGroup();
	}

	void RunTests() {
		TestX3();
		TestX3Cos();
		TestAckley();
	}
};

UtOptimizer test_Optimizer;  ///< Global instance automatically registers to UnitTestManager
