/**
* \file Optimizer.h
* \brief Declaration of class Optimizer and abstract callback class Optimizer::OptimizerFunc 
*/

#ifndef OPTIMIZER_H
#define OPTIMIZER_H

/**
* \ingroup SoarLib
* \brief Finds the x with minimal f(x) for functions defined using OptimizerFunc 
*
* <DIV class="groupHeader">Copyright (s)</DIV>
* - <B>Algon 60</B>: Brent, R. (1973) Algorithms for Minimization without Derivatives. Prentice-Hall.<BR/>
* - <B>Fortran</B>: fmin.f -- translated by f2c (version 19990503).<BR/>
*   D. Kahaner, C. Moler, S. Nash (1988) Numerical Methods and Software. Prentice Hall
* - <B>R</B> : A Computer Language for Statistical Data Analysis<BR/>
*   Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka<BR/>
*   Copyright (C) 2003-2004   The R Foundation
*   Copyright (C) 1998--2014  The R Core Team
*   (https://github.com/wch/r-source/blob/trunk/src/library/stats/src/optimize.c)
* - <B>C++</B> : Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de ) 
*
* <HR />
* <H2 class="groupheader">Include</H2>
*/
class Optimizer {

public:
	
	/**
	* \ingroup SoarLib
	* \brief Abstract base class for optimizer callback function classes
	* <HR />
	*
	* <H2 class="groupheader">Include and Inheritance</H2>
	*/
	class OptimizerFunc {
	public:
		virtual double operator()(double x) = 0;
	};
	
	/// \brief Finds the minimum of a function
	double Brent_fmin(double ax, double bx, OptimizerFunc &f, double tol);
};

#endif // OPTIMIZER_H
