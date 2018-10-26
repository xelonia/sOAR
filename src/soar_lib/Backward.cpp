/**
* \file Backward.cpp
* \brief Implementation of class Backward
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
*
*/

#include "Backward.h"

#pragma warning( disable : 4996)
#define _USE_MATH_DEFINES                      ///< Enforces that math.h adds M_PI and other constants
#include <iostream>
#include <math.h>

#define CALC_EPS       (1.0 / 100000.0)        ///< Epsilon, comparison with zero precision

#include "Decision.h"
#include "Optimizer.h"
#include "Settings.h"

Backward::Backward() {
    _decision = 0;
}

Backward::~Backward() {
}

void Backward::SetDecision(Decision *dec) {
    _decision = dec;
}


//--------------------------------------
// Interpolation between gridpoints

void Backward::Stoch_HMcN_AddStochNone(double x_case, double y_case, BwStochResultStruct &result)
{
    // Stochasticity for variable x (reserves)
	int    x_ln1_ind = (int)((x_case-_min_x) * _inverse_dx);	// Determine the first lower node
	if (x_ln1_ind >= (int)_x_cnt)
		x_ln1_ind  = _x_cnt-1;
	double x_ln1     = (_min_x + x_ln1_ind * _dx);			// value of 1st lower node, within range of grid
	//Same as:
	// double x_ln1 = _x_vec[x_ln1_ind];

	int x_un1_ind = x_ln1_ind + 1;							// index of 1st upper node
	if (x_un1_ind>=(int)_x_cnt)
		x_un1_ind = _x_cnt-1;

    // Set probabilities that the value of these nodes is taken:
    double p_x_un1 = (x_case-x_ln1)/_dx;			// probability of 1st upper node, linear case
    double p_x_ln1 = 1-p_x_un1;						// probability of 1st lower node, linear case  
	
     // Stochasticity for variable y (condition)
	int y_ln1_ind = (int)((y_case-_min_y) * _inverse_dy);	// Determine the first lower node
	if (y_ln1_ind >= (int)_y_cnt)
		y_ln1_ind  = _y_cnt-1;
	double y_ln1   = (_min_y + y_ln1_ind * _dy);			// value of 1st lower node, within range of grid
	//Same as:
	// double y_ln1 = _y_vec[y_ln1_ind];

	int y_un1_ind = y_ln1_ind + 1;							// index of 1st upper node
	if (y_un1_ind>=(int)_y_cnt)
		y_un1_ind = _y_cnt-1;

    // Set probabilities that the value of these nodes is taken:
    double p_y_un1 = (y_case-y_ln1)/_dy;					// probability of 1st upper node, linear case
    double p_y_ln1 = 1-p_y_un1;						// probability of 1st lower node, linear case
    
	// Interpolate f according to above stochasticity:
	result.curr_approx =
		p_x_ln1*p_y_ln1*_f_curr(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_curr(x_un1_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_curr(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_curr(x_un1_ind,y_ln1_ind) ;

	result.next_approx =
		p_x_ln1*p_y_ln1*_f_next(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_next(x_un1_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_next(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_next(x_un1_ind,y_ln1_ind) ;
}


void Backward::Stoch_HMcN_AddStochRes(double x_case, double y_case, BwStochResultStruct &result)
{
    // Stochasticity for variable x (reserves)
	int x_ln1_ind = (int)((x_case-_min_x) * _inverse_dx);	// Determine the first lower node
	if (x_ln1_ind >= (int)_x_cnt)
		x_ln1_ind  = _x_cnt-1;
	double x_ln1  = (_min_x + x_ln1_ind * _dx);			// value of 1st lower node, within range of grid
	//Same as:
	// double x_ln1 = _x_vec[x_ln1_ind];

	int x_ln2_ind = x_ln1_ind - 1;							// index of 2nd lower node
	if (x_ln2_ind<0)
		x_ln2_ind=0;

	int x_un1_ind = x_ln1_ind + 1;							// index of 1st upper node
	if (x_un1_ind>=(int)_x_cnt)
		x_un1_ind = _x_cnt-1;

	int x_un2_ind = x_ln1_ind + 2;							// index of 2nd upper node
	if (x_un2_ind>=(int)_x_cnt)
		x_un2_ind = _x_cnt-1;


    // Set probabilities that the value of these nodes is taken:
    double p_x_un1_lc = (x_case-x_ln1)/_dx;					// probability of 1st upper node, linear case
    double p_x_ln1_lc = 1-p_x_un1_lc;						// probability of 1st lower node, linear case
    
    // Control degree of stochasticity through stochfac alpha:
    double p_x_ln2 = _stochfac_x * p_x_ln1_lc;									 // probability of 2nd lower node
    double p_x_ln1 = (1-2*_stochfac_x) * p_x_ln1_lc + _stochfac_x * p_x_un1_lc;  // probability of 1st lower node
    double p_x_un1 = _stochfac_x * p_x_ln1_lc + (1-2*_stochfac_x) * p_x_un1_lc;  // probability of 1st upper node
    double p_x_un2 = _stochfac_x * p_x_un1_lc;									 // probability of 2nd upper node
    
    // Stochasticity for variable y (condition)
	int    y_ln1_ind = (int)((y_case-_min_y) * _inverse_dy);	// Determine the lower upper node
	if (y_ln1_ind >= (int)_y_cnt)
		y_ln1_ind  = _y_cnt-1;
	double y_ln1     = (_min_y + y_ln1_ind * _dy);			// value of 1st lower node, within range of grid
	int y_un1_ind    = y_ln1_ind + 1;						// index of first upper node
	if (y_un1_ind>=(int)_y_cnt)
		y_un1_ind = _y_cnt-1;


    // Set probabilities that the value of these nodes is taken:
    double p_y_un1 = (y_case-y_ln1)/_dy;                    // probability of 1st upper node  
    double p_y_ln1 = 1-p_y_un1;                             // probability of 1st lower node
    
	// Interpolate f according to above stochasticity:
	result.curr_approx =
		p_x_ln1*p_y_ln1*_f_curr(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_curr(x_un1_ind,y_un1_ind) +
		p_x_ln2*p_y_ln1*_f_curr(x_ln2_ind,y_ln1_ind) + p_x_ln2*p_y_un1*_f_curr(x_ln2_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_curr(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_curr(x_un1_ind,y_ln1_ind) +
		p_x_un2*p_y_ln1*_f_curr(x_un2_ind,y_ln1_ind) + p_x_un2*p_y_un1*_f_curr(x_un2_ind,y_un1_ind);

	result.next_approx =
		p_x_ln1*p_y_ln1*_f_next(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_next(x_un1_ind,y_un1_ind) +
		p_x_ln2*p_y_ln1*_f_next(x_ln2_ind,y_ln1_ind) + p_x_ln2*p_y_un1*_f_next(x_ln2_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_next(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_next(x_un1_ind,y_ln1_ind) +
		p_x_un2*p_y_ln1*_f_next(x_un2_ind,y_ln1_ind) + p_x_un2*p_y_un1*_f_next(x_un2_ind,y_un1_ind);
}


void Backward::Stoch_HMcN_AddStochHealth(double x_case, double y_case, BwStochResultStruct &result)
{
    // Stochasticity for variable x (reserves)
	int    x_ln1_ind = (int)((x_case-_min_x) * _inverse_dx);	// Determine the first lower node
	if (x_ln1_ind >= (int)_x_cnt)
		x_ln1_ind  = _x_cnt-1;
	double x_ln1     = (_min_x + x_ln1_ind * _dx);			// value of 1st lower node, within range of grid
	//Same as:
	// double x_ln1 = _x_vec[x_ln1_ind];

	int x_un1_ind = x_ln1_ind + 1;							// index of 1st upper node
	if (x_un1_ind>=(int)_x_cnt)
		x_un1_ind = _x_cnt-1;

    // Set probabilities that the value of these nodes is taken:
    double p_x_un1 = (x_case-x_ln1)/_dx;			// probability of 1st upper node, linear case
    double p_x_ln1 = 1-p_x_un1;						// probability of 1st lower node, linear case  
	
     // Stochasticity for variable y (condition)
	int y_ln1_ind = (int)((y_case-_min_y) * _inverse_dy);	// Determine the first lower node
	if (y_ln1_ind >= (int)_y_cnt)
		y_ln1_ind  = _y_cnt-1;
	double y_ln1   = (_min_y + y_ln1_ind * _dy);			// value of 1st lower node, within range of grid
	//Same as:
	// double y_ln1 = _y_vec[y_ln1_ind];

	int y_ln2_ind = y_ln1_ind - 1;							// index of 2nd lower node
	if (y_ln2_ind<0)
		y_ln2_ind=0;

	int y_un1_ind = y_ln1_ind + 1;							// index of 1st upper node
	if (y_un1_ind>=(int)_y_cnt)
		y_un1_ind = _y_cnt-1;

	int y_un2_ind = y_ln1_ind + 2;							// index of 2nd upper node
	if (y_un2_ind>=(int)_y_cnt)
		y_un2_ind = _y_cnt-1;

    // Set probabilities that the value of these nodes is taken:
    double p_y_un1_lc = (y_case-y_ln1)/_dy;					// probability of 1st upper node, linear case
    double p_y_ln1_lc = 1-p_y_un1_lc;						// probability of 1st lower node, linear case
    
    // Control degree of stochasticity through stochfac alpha:
    double p_y_ln2 = _stochfac_x * p_y_ln1_lc;									 // probability of 2nd lower node
    double p_y_ln1 = (1-2*_stochfac_x) * p_y_ln1_lc + _stochfac_x * p_y_un1_lc;  // probability of 1st lower node
    double p_y_un1 = _stochfac_x * p_y_ln1_lc + (1-2*_stochfac_x) * p_y_un1_lc;  // probability of 1st upper node
    double p_y_un2 = _stochfac_x * p_y_un1_lc;									 // probability of 2nd upper node
    
	// Interpolate f according to above stochasticity:
	result.curr_approx =
		p_x_ln1*p_y_ln1*_f_curr(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_curr(x_un1_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_curr(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_curr(x_un1_ind,y_ln1_ind) +
		p_x_ln1*p_y_ln2*_f_curr(x_ln1_ind,y_ln2_ind) + p_x_un1*p_y_un2*_f_curr(x_un1_ind,y_un2_ind) +
		p_x_ln1*p_y_un2*_f_curr(x_ln1_ind,y_un2_ind) + p_x_un1*p_y_ln2*_f_curr(x_un1_ind,y_ln2_ind) ;

	result.next_approx =
		p_x_ln1*p_y_ln1*_f_next(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_next(x_un1_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_next(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_next(x_un1_ind,y_ln1_ind) +
		p_x_ln1*p_y_ln2*_f_next(x_ln1_ind,y_ln2_ind) + p_x_un1*p_y_un2*_f_next(x_un1_ind,y_un2_ind) +
		p_x_ln1*p_y_un2*_f_next(x_ln1_ind,y_un2_ind) + p_x_un1*p_y_ln2*_f_next(x_un1_ind,y_ln2_ind) ;
}


void Backward::Stoch_HMcN_AddStochResHealth(double x_case, double y_case, BwStochResultStruct &result)
{
    // Stochasticity for variable x (reserves)
	int    x_ln1_ind = (int)((x_case-_min_x) * _inverse_dx);	// Determine the first lower node
	if (x_ln1_ind >= (int)_x_cnt)
		x_ln1_ind  = _x_cnt-1;
	double x_ln1     = (_min_x + x_ln1_ind * _dx);			// value of 1st lower node, within range of grid
	//Same as:
	// double x_ln1 = _x_vec[x_ln1_ind];

	int x_ln2_ind = x_ln1_ind - 1;							// index of 2nd lower node
	if (x_ln2_ind<0)
		x_ln2_ind=0;

	int x_un1_ind = x_ln1_ind + 1;							// index of 1st upper node
	if (x_un1_ind>=(int)_x_cnt)
		x_un1_ind = _x_cnt-1;

	int x_un2_ind = x_ln1_ind + 2;							// index of 2nd upper node
	if (x_un2_ind>=(int)_x_cnt)
		x_un2_ind = _x_cnt-1;


    // Set probabilities that the value of these nodes is taken:
    double p_x_un1_lc = (x_case-x_ln1)/_dx;					// probability of 1st upper node, linear case
    double p_x_ln1_lc = 1-p_x_un1_lc;						// probability of 1st lower node, linear case
    
    // Control degree of stochasticity through stochfac alpha:
    double p_x_ln2 = _stochfac_x * p_x_ln1_lc;									 // probability of 2nd lower node
    double p_x_ln1 = (1-2*_stochfac_x) * p_x_ln1_lc + _stochfac_x * p_x_un1_lc;  // probability of 1st lower node
    double p_x_un1 = _stochfac_x * p_x_ln1_lc + (1-2*_stochfac_x) * p_x_un1_lc;  // probability of 1st upper node
    double p_x_un2 = _stochfac_x * p_x_un1_lc;									 // probability of 2nd upper node
    
	
     // Stochasticity for variable y (condition)
	int y_ln1_ind = (int)((y_case-_min_y) * _inverse_dy);	// Determine the first lower node
	if (y_ln1_ind >= (int)_y_cnt)
		y_ln1_ind  = _y_cnt-1;
	double y_ln1   = (_min_y + y_ln1_ind * _dy);			// value of 1st lower node, within range of grid
	//Same as:
	// double y_ln1 = _y_vec[y_ln1_ind];

	int y_ln2_ind = y_ln1_ind - 1;							// index of 2nd lower node
	if (y_ln2_ind<0)
		y_ln2_ind=0;

	int y_un1_ind = y_ln1_ind + 1;							// index of 1st upper node
	if (y_un1_ind>=(int)_y_cnt)
		y_un1_ind = _y_cnt-1;

	int y_un2_ind = y_ln1_ind + 2;							// index of 2nd upper node
	if (y_un2_ind>=(int)_y_cnt)
		y_un2_ind = _y_cnt-1;

    // Set probabilities that the value of these nodes is taken:
    double p_y_un1_lc = (y_case-y_ln1)/_dy;					// probability of 1st upper node, linear case
    double p_y_ln1_lc = 1-p_y_un1_lc;						// probability of 1st lower node, linear case
    
    // Control degree of stochasticity through stochfac alpha:
    double p_y_ln2 = _stochfac_x * p_y_ln1_lc;									 // probability of 2nd lower node
    double p_y_ln1 = (1-2*_stochfac_x) * p_y_ln1_lc + _stochfac_x * p_y_un1_lc;  // probability of 1st lower node
    double p_y_un1 = _stochfac_x * p_y_ln1_lc + (1-2*_stochfac_x) * p_y_un1_lc;  // probability of 1st upper node
    double p_y_un2 = _stochfac_x * p_y_un1_lc;									 // probability of 2nd upper node
    
	// Interpolate f according to above stochasticity:
	result.curr_approx =
		p_x_ln1*p_y_ln1*_f_curr(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_curr(x_un1_ind,y_un1_ind) +
		p_x_ln2*p_y_ln1*_f_curr(x_ln2_ind,y_ln1_ind) + p_x_ln2*p_y_un1*_f_curr(x_ln2_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_curr(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_curr(x_un1_ind,y_ln1_ind) +
		p_x_un2*p_y_ln1*_f_curr(x_un2_ind,y_ln1_ind) + p_x_un2*p_y_un1*_f_curr(x_un2_ind,y_un1_ind) +
		p_x_ln1*p_y_ln2*_f_curr(x_ln1_ind,y_ln2_ind) + p_x_un1*p_y_un2*_f_curr(x_un1_ind,y_un2_ind) +
		p_x_ln2*p_y_ln2*_f_curr(x_ln2_ind,y_ln2_ind) + p_x_ln2*p_y_un2*_f_curr(x_ln2_ind,y_un2_ind) +
		p_x_ln1*p_y_un2*_f_curr(x_ln1_ind,y_un2_ind) + p_x_un1*p_y_ln2*_f_curr(x_un1_ind,y_ln2_ind) +
		p_x_un2*p_y_ln2*_f_curr(x_un2_ind,y_ln2_ind) + p_x_un2*p_y_un2*_f_curr(x_un2_ind,y_un2_ind);

	result.next_approx =
		p_x_ln1*p_y_ln1*_f_next(x_ln1_ind,y_ln1_ind) + p_x_un1*p_y_un1*_f_next(x_un1_ind,y_un1_ind) +
		p_x_ln2*p_y_ln1*_f_next(x_ln2_ind,y_ln1_ind) + p_x_ln2*p_y_un1*_f_next(x_ln2_ind,y_un1_ind) +
		p_x_ln1*p_y_un1*_f_next(x_ln1_ind,y_un1_ind) + p_x_un1*p_y_ln1*_f_next(x_un1_ind,y_ln1_ind) +
		p_x_un2*p_y_ln1*_f_next(x_un2_ind,y_ln1_ind) + p_x_un2*p_y_un1*_f_next(x_un2_ind,y_un1_ind) +
		p_x_ln1*p_y_ln2*_f_next(x_ln1_ind,y_ln2_ind) + p_x_un1*p_y_un2*_f_next(x_un1_ind,y_un2_ind) +
		p_x_ln2*p_y_ln2*_f_next(x_ln2_ind,y_ln2_ind) + p_x_ln2*p_y_un2*_f_next(x_ln2_ind,y_un2_ind) +
		p_x_ln1*p_y_un2*_f_next(x_ln1_ind,y_un2_ind) + p_x_un1*p_y_ln2*_f_next(x_un1_ind,y_ln2_ind) +
		p_x_un2*p_y_ln2*_f_next(x_un2_ind,y_ln2_ind) + p_x_un2*p_y_un2*_f_next(x_un2_ind,y_un2_ind);
}

//--------------------------------------
// payoff functions

// general payoff 
double Backward::H_nmg (double x, double y, double u, int o, double f_currexp, double f_nextexp) {
	return( S(x,y,o,u) * ((1-_p_exp) * f_currexp + _p_exp * f_nextexp) );
}

double Backward::H_mg (double x, double y, double f_currexp, double f_nextexp) {
	return( S_mig(x,y) * ((1-_p_exp) * f_currexp + _p_exp * f_nextexp) );
}

// specific payoffs
double Backward::H_nc (double x, double y, int e, int a, int o, int s, int t, double u) {
    double x_nc_val = X_nc(x,e,a,o,u,t);
    double y_ns_val = Y_ns(x,y,u,t);

	BwStochResultStruct res;
	Stoch_HMcN(x_nc_val, y_ns_val, res);
    
    return H_nmg(x, y, u, o, res.curr_approx, res.next_approx);
}

double Backward::H_s (double x, double y, int e, int a, int o, int s, int t, double u) {
    double x_s_val = X_s(x,e,a,o,u,t);
    double y_s_val = Y_s(x,y,u,t) ;
	
	BwStochResultStruct res;
	Stoch_HMcN(x_s_val, y_s_val, res);
    
    return H_nmg(x, y, u, o, res.curr_approx, res.next_approx);
}

double Backward::H_c (double x, double y, int e, int a, int o, int s, int t, double u) {
    double x_c_val  = X_c(x,e,a,o,u,t);
    double y_ns_val = Y_ns(x,y,u,t);    

	BwStochResultStruct res;
	Stoch_HMcN(x_c_val, y_ns_val, res);

    return H_nmg(x, y, u, o, res.curr_approx, res.next_approx);
}

double Backward::H_m (double x, double y, int e, int a, int o, int s, int t, double u) {
	double x_m_val = X_m(x,e,a,o,s,u,t);
	double y_m_val = Y_m(x,y,e,o,s,u,t);
	
	BwStochResultStruct res;
	Stoch_HMcN(x_m_val, y_m_val, res);

	return H_mg(x, y, res.curr_approx, res.next_approx);
}

bool Backward::InitBackward(Settings *settings, double theta) {

    if (!_decision) {
        printf("Backward::InitBackward() ERROR: 'decision' not initialized\n");
		return false;
    }
    
    // prepare parameters
    
	InitStateFuncs(settings,theta);
    
    _decision->SetTheta(theta);		// Copy value, to be used in Forward
    _decision->SetLambda(0.0);      // Initial value
    
    _n                = settings->GetN();
	_n_min            = settings->GetNMin();
    _t_cnt            = settings->GetTCnt();
    _n_brood          = settings->GetNBrood();
    _gamma_nest       = settings->GetGammaNest();
    _crit             = settings->GetCrit();    
    
    _a_max            = settings->GetAMax();
    _p_exp            = settings->GetPexp();
    
    _e_cnt            = settings->GetECnt();
    _a_cnt            = settings->GetACnt();
	_o_cnt            = settings->GetOCnt();
	_s_cnt            = settings->GetSCnt();
    
    _x_cnt            = settings->GetXCnt();
    _y_cnt            = settings->GetYCnt();

    _dx               = settings->GetDx();
    _dy               = settings->GetDy();
	_inverse_dx       = 1.0 / _dx; 
	_inverse_dy       = 1.0 / _dy;
    
    _stochfac_x       = settings->GetStochFacX();
 
	_min_x            = settings->GetXMin();
	_min_y            = settings->GetYMin();

	_x_vec.Init(_x_cnt);
	_y_vec.Init(_y_cnt);
	
    for (unsigned int x=0;x<_x_cnt;x++)
        _x_vec[x] = _min_x+ x*_dx;
    for (unsigned int y=0;y<_y_cnt;y++)
        _y_vec[y] = _min_y+ y*_dy;

	_x_indep = settings->GetXindep();
    _y_indep = settings->GetYindep();  

	_migr_dur = settings->GetMigrDur();

	// Allocate the arrays once and re-use them later
    _f_curr.Init(_x_cnt,_y_cnt);
    _f_next.Init(_x_cnt,_y_cnt);

    //----------------------------------------------
    // set up arrays and set terminal condition

    if ( !_decision->IsInitialized() ) {
        _decision->InitDimensions( _x_cnt, _y_cnt, _e_cnt, _a_cnt, _o_cnt, _s_cnt, _t_cnt );

		for (unsigned int x=0;x<_x_cnt;x++) {
			for (unsigned int y=0;y<_y_cnt;y++) {
				for (unsigned int e=0;e<_e_cnt;e++) {
					for (unsigned int a=0;a<_a_cnt;a++) {
						for (unsigned int o=0;o<_o_cnt;o++) {
							for (unsigned int s=0;s<_s_cnt;s++) {
								for (unsigned int t=0;t<_t_cnt;t++) {
									_decision->SetF(x,y,e,a,o,s,t, 1);
								} } } } } } }

		for (unsigned int y=0;y<_y_cnt;y++) {
			for (unsigned int e=0;e<_e_cnt;e++) {
				for (unsigned int a=0;a<_a_cnt;a++) {
					for (unsigned int o=0;o<_o_cnt;o++) {
						for (unsigned int s=0;s<_s_cnt;s++) {
							for (unsigned int t=0;t<_t_cnt;t++) {
								_decision->SetF_strat(0,y,e,a,o,s,t, 'n');
								_decision->SetF(0,y,e,a,o,s,t, 0);
							} } } } } }

    }  

	// Initialize the _stoch_hmcn_func function pointer to of the four grid interpolation functions
	// A call to Stoch_HMcN() then calls the selected function 
	unsigned int idx = ((settings->GetStochAddReserves()?1:0) + (settings->GetStochAddHealth()?2:0));
	switch(idx)
	{
	case 0: _stoch_hmcn_func = &Backward::Stoch_HMcN_AddStochNone;      break;
	case 1: _stoch_hmcn_func = &Backward::Stoch_HMcN_AddStochRes;       break;
	case 2: _stoch_hmcn_func = &Backward::Stoch_HMcN_AddStochHealth;    break;
	case 3: _stoch_hmcn_func = &Backward::Stoch_HMcN_AddStochResHealth; break;
	}

	return true;
}


//---------------------------------------

void Backward::CalcLambdaAndConvergence(NArray<double> &f_old , double lambda_old, BwConvResultStruct & result) {
	double lambda_worst = 1;		// Lambda value with highest difference to optimal 1.0
	double lambda_max_delta = 0;	// Highest deviation against optimal lambda of 1
	double lambda_average = 0;		// Average lambda over all state combinations
	int    state_count = 0;			// Count of state combinations used for average calcualtion
	int    notconv_count = 0;

	for (unsigned int x=1;x<_x_cnt;x++) {
		for (unsigned int y=0;y<_y_cnt;y++) {
			for (unsigned int e=0;e<_e_cnt;e++) {
				for (unsigned int a=0;a<_a_cnt;a++)	{
					for (unsigned int o=0; o<_o_cnt; o++) {
						for (unsigned int s=0; s<_s_cnt; s++) {

							unsigned int t = 0;	
							double oldVal = f_old(x,y,e,a,o,s,t);
							double newVal = _decision->GetF(x,y,e,a,o,s,t);	
								
							if (oldVal > 0)
							{
								double lambda = newVal / oldVal;

								double lambda_delta = fabs(lambda - 1.0);
								if (lambda_max_delta < lambda_delta) {
									lambda_max_delta = lambda_delta;
									lambda_worst = lambda;
								}

								// Update average
								lambda_average += lambda;
								state_count++;

								if (lambda_delta >= _crit) 
								{
									notconv_count++;
								}
							}
						}
					}
				}
			}
		}
	}
	lambda_average = lambda_average / state_count;

	result.lambda_bw_average = lambda_average;
    result.lambda_bw_worst   = lambda_worst;
	result.bw_notconv_count = notconv_count;
	result.bw_state_count    = state_count;
	result.bw_convergence = lambda_max_delta < _crit;

	// For comparison (lambda calculated for particular state)
	double oldVal     =           f_old(_x_cnt-1,_y_cnt-1,_e_cnt-1,0,0,0,_t_cnt-1);
	double newVal     = _decision->GetF(_x_cnt-1,_y_cnt-1,_e_cnt-1,0,0,0,_t_cnt-1);
	result.lambda_bw_state  = (double) newVal/oldVal;;
}


void Backward::UpdateFCurrFNext(int e, int a, int o, int s, int t) {
	int en = Chop(e+1,0,(_e_cnt-1));

	for (unsigned int xi=0;xi<_x_cnt;xi++) { 
		for (unsigned int yi=0;yi<_y_cnt;yi++) {
			_f_curr(xi,yi) = _decision->GetF(xi,yi, e ,a,o,s,t);
			_f_next(xi,yi) = _decision->GetF(xi,yi, en,a,o,s,t);
		}
	}
}


/// Compute H no care for current state
void Backward::ComputeHNoCare(int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result ) {
	BwOptimizerFunc u_min_nc( _x_vec[res], _y_vec[cond], ex, age, loc, s, week, this, &Backward::H_nc);

	result.u = _optimizer.Brent_fmin(0.0,1.0, u_min_nc, 1.0e-10);
	result.H = H_nc(_x_vec[res], _y_vec[cond], ex, age, loc, s, week, result.u);
	result.s = 'n';
}


/// Compute H start for current state
void Backward::ComputeHStart(int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result ) {
	BwOptimizerFunc u_min_s( _x_vec[res], _y_vec[cond], ex, age, loc, s, week, this, &Backward::H_s);

	result.u = _optimizer.Brent_fmin(0.0,1.0, u_min_s, 1.0e-10);
	result.H = H_s(_x_vec[res], _y_vec[cond], ex, age, loc, s, week, result.u);
	result.s = 's';
}


/// Compute H care for current state
void Backward::ComputeHCare(int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result ) {
	double opt_min = U_crit(ex, age, _loc_idx, week); // Limit Brent_fmin optimizer search range to [opt_min - 1.0]

	BwOptimizerFunc u_min_c( _x_vec[res], _y_vec[cond], ex, age, loc, s, week, this, &Backward::H_c);

	result.u = _optimizer.Brent_fmin(opt_min,1.0, u_min_c, 1.0e-10);
	result.H = H_c(_x_vec[res], _y_vec[cond], ex, age, loc, s, week,result.u);
	result.s = 'c';
}


/// Compute H migrate for current state
void Backward::ComputeHMigrate(int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result ) {

	double u_opt_m=0;
	double H_migrate = H_m(_x_vec[res], _y_vec[cond], ex, age,loc, s, week, u_opt_m);

	result.H = H_migrate;
	result.u = u_opt_m;
	result.s = 'm';
}



double Backward::Compute(Settings *settings, double theta) {

	if (!InitBackward(settings, theta) )
		return 0;

	double lambda       = 0;
	double lambda_old   = 0;
	int year            = 0; 
	int yearTotal       = _decision->GetYear();
	bool converged      = false;

	double u_opt_m=0;
    
    NArray<double> f_old;
    f_old.Init(  _x_cnt, _y_cnt, _e_cnt, _a_cnt, _o_cnt, _s_cnt, _t_cnt );
        

    //-----------------------------------------------
    // ITERATION
    
	while ((!converged && year<_n) || year<_n_min)
	{
		year++;
		yearTotal++;

		// f_old NArray is filled with member-wise copy of current decision f NArray
        f_old = _decision->GetF();  
        
        for (int week=(_t_cnt-1);week>=0;week--)
        {

#ifdef BW_TIMING
            _timer_week.Clear();
            _timer_week.Start();
#endif
            
            // set week t_cnt = week0; note that definition is different from R and Matlab here because index starts at 0
            int week_next = (week+1)%_t_cnt;
            
            // Calculate best strategy for age < age_max
            
            for (unsigned int res=1;res<_x_cnt;res++) 
			{
                // for (unsigned int cond=1;cond<_y_cnt;cond++)   // original
				for (unsigned int cond=0;cond<_y_cnt;cond++)
				{   
					for (_loc_idx=0;_loc_idx<_o_cnt;_loc_idx++) 
					{						
						for (unsigned int ex=0;ex<_e_cnt;ex++) 
						{              

							BwOptResultStruct opt_nocare, opt_start, opt_migrate;

							// no care
							UpdateFCurrFNext(ex,0,_loc_idx,0,week_next);				          // UpdateFCurrFNext(e,a,o,s,t) -> f of state at t+1 when 'nocare' is performed			
							ComputeHNoCare(res,cond, ex, 0, _loc_idx, 0, week, opt_nocare);       // -> u_opt_nc, H_nocare

							// start brood
							UpdateFCurrFNext(ex,1,_loc_idx,0,week_next);                          // UpdateFCurrFNext(e,a,o,s,t) -> f of state at t+1 when 'start' is performed
							ComputeHStart(res,cond, ex, 0, _loc_idx, 0, week, opt_start);         // -> u_opt_s, H_start

							// migrate
							if (_migr_dur > 1) {                                                  // Duration of migration longer than 1 week (bird won't be at other location at t+1)
								UpdateFCurrFNext(ex,0,_loc_idx,1,week_next);			          // UpdateFCurrFNext(e,a,o,s,t) -> f of state at t+1 when 'migrate' is performed
							}
							else {	                                                              // duration of migration equals one decision epoch (bird will be in other location at t+1)
								UpdateFCurrFNext(ex,0,(_loc_idx+1)%_o_cnt,0,week_next);           // UpdateFCurrFNext(e,a,o,s,t) -> f of state at t+1 when 'migrate' is performed	
							}
							ComputeHMigrate(res,cond, ex, 0, _loc_idx, 0, week, opt_migrate);     // -> u_opt_m, H_migrate

							
							// extract best strategy and corresponding f and u
							BwOptResultStruct *opt_best = &opt_nocare;

							if (opt_migrate.H > opt_start.H && opt_migrate.H > opt_nocare.H && fabs(opt_migrate.H)>=CALC_EPS)
								opt_best = &opt_migrate;
							else if (opt_start.H > opt_nocare.H && fabs(opt_start.H)>=CALC_EPS) 
								opt_best = &opt_start;

							_decision->SetF_all(res,cond,ex,0,_loc_idx,0,week,  opt_best->H, opt_best->u,opt_best->s);


							// Migration longer than one decision epoch
							if (_migr_dur > 1) {
							
								// loop over duration of migration (except final week of migration)
								for (int dur=1; dur<_migr_dur-1; dur++) { 
									UpdateFCurrFNext(ex,0,_loc_idx,dur+1,week_next);
									ComputeHMigrate(    res,cond,ex,0,_loc_idx,dur,week, opt_migrate);									
									_decision->SetF_all(res,cond,ex,0,_loc_idx,dur,week, opt_migrate.H,opt_migrate.u,opt_migrate.s); 
								}

								// last week of migration							
								UpdateFCurrFNext(ex,0,(_loc_idx+1)%_o_cnt,0,week_next);								
								ComputeHMigrate(    res,cond,ex,0,_loc_idx,_s_cnt-1,week,  opt_migrate);							
								_decision->SetF_all(res,cond,ex,0,_loc_idx,_s_cnt-1,week,  opt_migrate.H,opt_migrate.u,opt_migrate.s); 
							}

							// loop over brood
							for (unsigned int age=1;age<(_a_cnt-1);age++) 
							{
								// if u_crit > 1, forced to abandon brood
								if (U_crit(ex, age, _loc_idx, week) > 1) {
									_decision->SetF_all(res,cond,ex,age,_loc_idx,0,week, opt_nocare.H,opt_nocare.u,opt_nocare.s); 
								}
								else  // else care for brood
								{
									BwOptResultStruct opt_care;

									UpdateFCurrFNext(ex,age+1,_loc_idx,0,week_next);    
									ComputeHCare(res,cond, ex, age, _loc_idx, 0, week, opt_care);

									// Decide if care or no care
									BwOptResultStruct *opt_best = &opt_nocare;
									if (opt_care.H >= opt_nocare.H && fabs(opt_care.H)>=CALC_EPS)
										opt_best = &opt_care;
									_decision->SetF_all(res,cond,ex,age,_loc_idx,0,week,   opt_best->H,opt_best->u,opt_best->s);
                                
								} //end care for brood
                            
							} // end loop over brood
                        
						} // end loop over experience 'ex' for age < age_max
					} // end loop over locations '_loc_idx' for age < age_max
                } // end loop over condition 'cond' for age < age_max
            } // end loop over reserves 'res' for age < age_max
            
            
            // Calculate best strategy for age = age_max            
            for (unsigned int res=1;res<_x_cnt;res++) {
                // for (unsigned int cond=1;cond<_y_cnt;cond++) {  // original
				for (unsigned int cond=0;cond<_y_cnt;cond++) {
					for (unsigned int _loc_idx=0;_loc_idx<_o_cnt;_loc_idx++) {
						for (unsigned int ex=0;ex<_e_cnt;ex++) {
                        
							BwOptResultStruct opt_nocare;

							// no care							
							UpdateFCurrFNext(ex,0,_loc_idx,0,week_next); 
							ComputeHNoCare(res,cond, ex, 0, _loc_idx, 0, week, opt_nocare);
                        
							// brood becomes independent							
							UpdateFCurrFNext(0   ,0,_loc_idx,0,week);

							
							BwStochResultStruct stoch;
							Stoch_HMcN(_x_indep, _y_indep, stoch);
                        
							double fVal = opt_nocare.H + _n_brood * stoch.curr_approx;

							_decision->SetF_all(res,cond,ex,(_a_cnt-1),_loc_idx,0,week,   fVal,opt_nocare.u, opt_nocare.s); 
						} // end loop over  'ex'       for a=a_max
					} // end loop over  '_loc_idx' for a=a_max
                } // end loop over  'cond'     for a=a_max
            } // end loop over  'res'      for a=a_max
            
#ifdef BW_TIMING
            printf("====================== Backward Cycle %d / Decision epoch %d ===========\n",yearTotal, week);

            _timer_week.Stop();
            printf("Decision epochs total %12.2f ns\n",_timer_week.GetNanoSeconds() );
#endif

        } // end loop over weeks
        
        
        // calculate lambda
		BwConvResultStruct conv;

		CalcLambdaAndConvergence(f_old, lambda_old, conv);

		lambda     = conv.lambda_bw_state;   		
		converged  = conv.bw_convergence;
		lambda_old = lambda;


		printf("=========== Backward iteration %2d  done ===========\n",yearTotal);	 
		printf("........... Lambda:  average = %f   specific state = %f\n",conv.lambda_bw_average,conv.lambda_bw_state);
		printf("........... Lambda:  worst   = %f   not converged %d of %d \n", conv.lambda_bw_worst,conv.bw_notconv_count,conv.bw_state_count );
		printf("\n");

		_decision->SetYear(yearTotal);
		_decision->SetLambda(conv.lambda_bw_average);
		_decision->SetLambdaState(conv.lambda_bw_state);
		_decision->SetLambdaWorst(conv.lambda_bw_worst);     
		_decision->SetBwNotConvCount(conv.bw_notconv_count); 
		_decision->SetBwStateCount(conv.bw_state_count);     
		_decision->SetConvergence(converged);


	} // end while loop over years and dlambda
    
    _decision->SetLambda(lambda);
    return lambda;
}
