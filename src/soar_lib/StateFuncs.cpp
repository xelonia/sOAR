/**
* \file StateFuncs.cpp
* \brief Implementation of class StateFuncs
*
*
*  Created by Damaris Zurell on 23.03.15.
*  Enhancements and bugfixes by Merlin Schaefer and Karsten Isakovic.
*  Copyright (C) 2015-2016 Damaris Zurell (damaris.zurell@wsl.ch), 
*  Merlin Schaefer (mschaefer@uni-potsdam.de). All rights reserved.
*
*  This file is part of sOAR.
*
*  sOAR is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  sOAR is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with sOAR.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "Settings.h"
#include "StateFuncs.h"

#define _USE_MATH_DEFINES
#include <math.h>
//#include <cstdlib>
#include <string.h>


//---------------------------------------
// helper functions


// Chop
double StateFuncs::Chop(double x, double minx, double maxx)
{
    if (x<minx)
        return minx;
    if (x>maxx)
        return maxx;
    return x;
    // return(std::min(std::max(minx,x),maxx));
}

int StateFuncs::Chop(int x, int minx, int maxx) {
    if (minx>x)
        return minx;
    if (maxx<x)
        return maxx;
    return x;
}

// environment
double StateFuncs::Env(int o, int t) {
	if (_env_food_supply.GetSize()==0) {
	    return(_eps[o] * sin( (t - (_t_max / 4.0)) * 2.0 * M_PI / _t_max ) + _a_bar[o]) * (_x_max - _x_min);
	}
	else {
		return _env_food_supply(t,o);
	}
}

// Gamma
double StateFuncs::Gamma(int e, int o, double u, int t) {
    return( pow(_theta, (int)(_e_max-e)) * Env(o,t) * u );
}

// Gamma brood
double StateFuncs::GammaBrood(int a) {
    return(_gamma_brood_ind[a]);
}

// U_crit
double StateFuncs::U_crit(int e, int a, int o, int t) {
    if (_theta > 0)
        return( GammaBrood(a) * pow(_theta,e-(int)_e_max) / (Gamma(e,o,1,t) / pow(_theta,(int)(_e_max-e))));
    else return (2);
}

// C (metabolism)
double StateFuncs::C_u(double x, double _x_max, double u) {
	double x_norm = (x/_x_max);
	return ( _c_bmr * (1 + _c_bmr_x_func(x_norm) ) + _c_u_func(u) * (1+ _c_x_func(x_norm) ) );
}

// alpha (change of condition)
double StateFuncs::Alpha(double c) {
	return ( _alpha_func(c) );
} 

// Probability of flight mode (active flight)
double StateFuncs::P_active_flight(int o, int s, int t) { 
	if (_p_active_flight.GetSize() == 0)
		return _p_active_flight_const;

	int loc;
	if (o==0) {
		loc = s;
	}
	else {
		loc = (_p_active_flight.GetDim(1)-1) - s;
	}
	return _p_active_flight(t,loc);
}

// D
double StateFuncs::D(double y) {
	return ( _d_func(y) );
} 

// M, i.e. predation
double StateFuncs::M(int o, double u, double x, double _x_max) {
	double x_norm = (x/_x_max);
	return ( _m_u_func[o](u) * (1+ _m_x_func[o](x_norm) ) );
}


// M during migration
double StateFuncs::M_mig(double x) {  
	double x_norm = (x/_x_max);
	return ( _m_migr * (1+ _m_migr_x_func(x_norm)) );
}

// S
double StateFuncs::S(double x, double y, int o, double u) {
	return ( (1 - D(y)) * (1 - M(o,u,x,_x_max)) );	
}

// S_mig
double StateFuncs::S_mig(double x, double y) {
	return (1 - M_mig(x)) * (1 - D(y));
}

//---------------------------------------
// state variable functions

double StateFuncs::X_nc(double x, int e, int a, int o, double u, int t) {
    return(Chop(x + Gamma(e,o,u,t) - C_u(x,_x_max,u), _x_min, _x_max));
}

double StateFuncs::X_c (double x, int e, int a, int o, double u, int t) {
    return(Chop(x + Gamma(e,o,u,t) - GammaBrood(a) - C_u(x,_x_max,u), _x_min, _x_max));
}

double StateFuncs::X_s (double x, int e, int a, int o, double u, int t) {
    return(Chop(x + Gamma(e,o,u,t) - _delta_res_start - C_u(x,_x_max,u), _x_min, _x_max));
}

double StateFuncs::X_m (double x, int e, int a, int o, int s, double u, int t) {
	double p_act = P_active_flight(o,s,t);

	return(Chop(x - ( p_act * (_dres_migr_act( e-(int)_e_max)) * (1 + _migr_act_x_func(x)) + (1-p_act) * (_dres_migr_pas(e-(int)_e_max)) * (1 + _migr_pas_x_func(x)) ) , _x_min, _x_max));
}

double StateFuncs::Y_s (double x, double y, double u, int t) {
    return(Chop(y + Alpha(C_u(x,_x_max,u)) - _delta_cond_start, _y_min, _y_max));
}

double StateFuncs::Y_ns (double x, double y, double u, int t) {
    return(Chop(y + Alpha(C_u(x,_x_max,u)), _y_min, _y_max));
}

double StateFuncs::Y_m (double x, double y, int e, int o, int s, double u, int t) {	
	double p_act = P_active_flight(o,s,t);
	return(Chop(y - ( p_act * _dcond_migr_act(e-(int)_e_max) + (1-p_act) * _dcond_migr_pas(e-(int)_e_max) ) , _y_min, _y_max));
}


void StateFuncs::InitStateFuncs(Settings *settings, double theta) {

    _theta = theta;

	_eps              = settings->GetEps();          
	_a_bar            = settings->GetABar();
	
    _x_min            = settings->GetXMin();
    _x_max            = settings->GetXMax();
    _y_min            = settings->GetYMin();
    _y_max            = settings->GetYMax();
	
	int incub = settings->GetIncubation();    
	int a_cnt = settings->GetACnt();
	_gamma_brood_ind = new double[ a_cnt ];
    _gamma_brood_ind[0] = 0;
	for (int a=1;a<a_cnt;a++) {
		if (a <= incub)
			_gamma_brood_ind[a]=settings->GetGammaIncub();
		else
			_gamma_brood_ind[a]=settings->GetGammaNest();
	}

    _e_max            = settings->GetEMax();
	_t_max            = settings->GetTCnt();

	_m_migr           = settings->GetMMigr();
	_dres_migr_act    = settings->DResMigrAct();
	_dres_migr_pas    = settings->DResMigrPas();
	_dcond_migr_act   = settings->DCondMigrAct();
	_dcond_migr_pas   = settings->DCondMigrPas();
	
    _delta_res_start  = settings->GetDeltaResStart();
    _delta_cond_start = settings->GetDeltaCondStart();

	_c_bmr             = settings->GetCBmr();
	_c_bmr_x_func      = settings->CFuncBmrX();
	_c_u_func          = settings->CFuncU();
	_c_x_func          = settings->CFuncX();
	_m_u_func[0]       = settings->MFuncU(0);
	_m_x_func[0]       = settings->MFuncX(0);
	_m_u_func[1]       = settings->MFuncU(1);
	_m_x_func[1]       = settings->MFuncX(1);
	_m_migr_x_func     = settings->MMigFuncX();
	_alpha_func        = settings->AlphaFunc();
	_d_func            = settings->DFunc();
	_migr_act_x_func   = settings->MigActFuncX();
	_migr_pas_x_func   = settings->MigPasFuncX();

	_p_active_flight_const = settings->GetPActiveFlightConst();
	_p_active_flight       = settings->GetPActiveFlight();
	_env_food_supply       = settings->GetEnvFoodSupply();
}
   
void StateFuncs::PrintFuncs() {

	char c_debug[256];
	char a_debug[256];
	char d_debug[256];
	char m0_debug[256];
	char m1_debug[256];
	char mmigr_debug[256];

	strcpy_s(a_debug, sizeof(a_debug), _alpha_func.ToString("c") );
	strcpy_s(d_debug, sizeof(d_debug), _d_func.ToString("y") );
	
	strcpy_s(m0_debug, sizeof(m0_debug), _m_u_func[0].ToString("u") );
	if (!_m_x_func[0].IsZero() ) {
		strcat_s(m0_debug,sizeof(m0_debug)," * (1+ ");
		strcat_s(m0_debug,sizeof(m0_debug),_m_x_func[0].ToString("x") );
		strcat_s(m0_debug,sizeof(m0_debug)," )");
	}

	strcpy_s(m1_debug, sizeof(m1_debug), _m_u_func[1].ToString("u") );
	if (!_m_x_func[1].IsZero() ) {
		strcat_s(m1_debug,sizeof(m1_debug)," * (1+ ");
		strcat_s(m1_debug,sizeof(m1_debug),_m_x_func[1].ToString("x") );
		strcat_s(m1_debug,sizeof(m1_debug)," )");
	}

	sprintf_s(c_debug,sizeof(c_debug),"%g",_c_bmr );
	if (!_c_bmr_x_func.IsZero() ) {
		strcat_s(c_debug,sizeof(c_debug)," * (1 + ");
		strcat_s(c_debug,sizeof(c_debug), _c_bmr_x_func.ToString("x") );
		strcat_s(c_debug,sizeof(c_debug)," )");
	}
	strcat_s(c_debug,sizeof(c_debug)," + ");
	strcat_s(c_debug,sizeof(c_debug), _c_u_func.ToString("u") );
	if (!_c_x_func.IsZero() ) {
		strcat_s(c_debug,sizeof(c_debug)," * (1+ ");
		strcat_s(c_debug,sizeof(c_debug), _c_x_func.ToString("x") );
		strcat_s(c_debug,sizeof(c_debug)," ) ");
	}

	sprintf_s(mmigr_debug,sizeof(mmigr_debug),"%g",_m_migr );
	if (!_m_migr_x_func.IsZero() ) {
		strcat_s(mmigr_debug,sizeof(mmigr_debug)," * (1 + ");
		strcat_s(mmigr_debug,sizeof(mmigr_debug), _m_migr_x_func.ToString("x") );
		strcat_s(mmigr_debug,sizeof(mmigr_debug)," )");
	}

	printf("Alpha => %s\n",a_debug);
	printf("D     => %s\n",d_debug);
	printf("C     => %s\n",c_debug);
	printf("M0    => %s\n",m0_debug);
	printf("M1    => %s\n",m1_debug);
	printf("MMig  => %s\n",mmigr_debug);

}


StateFuncs::StateFuncs() {
	_gamma_brood_ind = 0;
}

StateFuncs::~StateFuncs() {	
    if (_gamma_brood_ind) {
        delete [] _gamma_brood_ind;
    }
}