sOAR is an application for computing optimal animal life-history strategies in cyclic environments. 

The source code was created in C++ by Damaris Zurell on 23.03.15. Enhancements and bugfixes by Merlin Schaefer and Karsten Isakovic, 2015-2017. Copyright (C) 2015-2017 Damaris Zurell (damaris.zurell@wsl.ch), Merlin Schaefer (mschaefer@uni-potsdam.de). All rights reserved.

sOAR is distributed under the terms of the GNU General publich License, version 3.0, which is supplied in the gnu_general_public_license_v3.txt file.

The source code can be found at
	https://sourceforge.net/projects/soar-animal-behaviour/

The vs_2008 and vs_2010 folders contain the solutions for the free integrated development environments (IDE) "Visual C++ 2008 Express" and "Visual C++ 2010 Express" developed by Microsoft.


sOAR incorporates code from Karsten Isakovic, the library Libconfig and a Brent optimizer. 


Libconfig is a simple library for processing structured configuration files and is distributed under the terms of the GNU Lesser General Public License, which is supplied in the accompanying COPYING.LIB file under \extern\libconfig\.

The source code can be found at
	http://www.hyperrealm.com/libconfig/
	
The lib_vs2008 and lib_vs2010 folders contain the binary versions (lib and dll) for libconfig compiled with VisualStudio 2008 and 2010 which are needed to compile the OAR support lib.


The Brent optimizer is based on Brent, R. (1973) Algorithms for Minimization without Derivatives, Prentice-Hall. The original C code was taken from "R: A Computer Language for Statistical Data Analysis" (https://github.com/wch/r-source/blob/trunk/src/library/stats/src/optimize.c) with Copyright (C) 1995, 1996  Robert Gentleman and Ross Ihaka, Copyright (C) 2003-2004 The R Foundation,   Copyright (C) 1998--2014 The R Core Team; then modified for C++ and sOAR by Karsten Isakovic, Copyright (C) 2016 Karsten Isakovic (Karsten.Isakovic@web.de).