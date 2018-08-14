/**
* \file Forward.cpp
* \brief Implementation of class Forward
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

#include "Forward.h"

#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <iterator>
#include <limits>

#include "Decision.h"
#include "Settings.h"
#include "Individual.h"

#pragma warning ( disable: 4996) // warning C4996: 'fopen': This function or variable may be unsafe.

#define CALC_PRECISION (100000.0)
#define CALC_EPS       (1.0 / CALC_PRECISION)

Forward::Forward() {
	_decision = 0;
}

Forward::~Forward() {
}

void Forward::SetDecision(Decision *dec) {
	_decision = dec;
}

//------------------------------

void   Forward::SavePopulationDynamics(char *filename)
{
	if (_FW_props.GetSize()==0)  {
		return;
	}

	FILE *file = fopen(filename,"wb");
	if (file==0) {
		printf("Forward::SavePopulationDynamics(%s) error opening the file for writing\n",filename);
		return;
	}    

	_FW_props.SaveBinary(file);

	fwrite(&_conv.lambda_fw_average,1,sizeof(double), file);
	fwrite(&_conv.fw_convergence,1,sizeof(bool), file);			
	fwrite(&_conv.fw_year_conv,1,sizeof(int), file);			
	fwrite(&_conv.lambda_fw_state,1,sizeof(double), file);      
	fwrite(&_conv.lambda_fw_worst,1,sizeof(double), file);		
	fwrite(&_conv.fw_notconv_count,1,sizeof(int), file);		
	fwrite(&_conv.fw_state_count,1,sizeof(int), file);			

	fclose(file);
}


bool Forward::LoadFwFromFile(char *filename) 
{
	FILE *file = fopen(filename,"rb");
	if (file==0) {
		printf("Forward::load_from_file(%s) error opening the file for reading\n",filename);
		return false;
	}

	if (!_FW_props.LoadBinary(file) ) {
		printf("Forward::load_from_file(%s) error file read error on 'FW_props'\n",filename);
		return false;
	}
	
	fread(&_conv.lambda_fw_average,1,sizeof(double), file); 
	fread(&_conv.fw_convergence,1,sizeof(bool), file);
	fread(&_conv.fw_year_conv,1,sizeof(int), file);
	fread(&_conv.lambda_fw_state,1,sizeof(double), file);
	fread(&_conv.lambda_fw_worst,1,sizeof(double), file);    
	fread(&_conv.fw_notconv_count,1,sizeof(int), file);     
	fread(&_conv.fw_state_count,1,sizeof(int), file);        

	fclose(file);

	_fw_initialized = true;

	return true;
}



// grid functions and stochasticity

// Function to interpolate between grid points and to introduce additional stochasticity following Houston and McNamara 1999, e.g. to prevent grid effects or to include additional environmental stochasticity. Default function: linear interpolation for energy reserves and health conditiond, additional stochasticity for energy reserves. 

// ------------------------------------------------------------------
// No additional stochasticity
// ------------------------------------------------------------------
void Forward::Stoch_HMcN_AddStochNone(double x_case, double y_case, FwStochXYPropResultStruct &cases)
{
	// Stochasticity for variable x (reserves)

	// Determine the first lower node:
	std::vector<double>::iterator X_un1_It  = std::upper_bound(_x_vec.begin(), _x_vec.end(), x_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int X_ln1_Ind = Chop((int) std::distance(_x_vec.begin(),X_un1_It)-1, 0, _x_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double X_ln1  = _x_vec[X_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other 3 surrounding nodes:
	int X_un1_Ind = Chop(X_ln1_Ind + 1, 0, _x_cnt-1);                          // index of 1st upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Xun1 = (x_case-X_ln1)/_dx;									   // probability of 1st upper node, linear case
	double p_Xln1 = 1-p_Xun1;											   // probability of 1st lower node, linear case

	// Stochasticity for variable y (condition)

	// Determine the first lower node:
	std::vector<double>::iterator Y_un1_It  = std::upper_bound(_y_vec.begin(), _y_vec.end(), y_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int Y_ln1_Ind = Chop((int) std::distance(_y_vec.begin(),Y_un1_It)-1, 0, _y_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double Y_ln1  = _y_vec[Y_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other surrounding node:
	int Y_un1_Ind = Chop(Y_ln1_Ind + 1, 0, _y_cnt-1);                          // index of 1st upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Yun1 = (y_case-Y_ln1)/_dy;									   // probability of 1st upper node, linear case
	double p_Yln1 = 1-p_Yun1;											   // probability of 1st lower node, linear case


	// return results:
	cases.x_grid[0] = X_ln1_Ind;
	cases.x_grid[1] = X_un1_Ind;

	cases.y_grid[0] = Y_ln1_Ind;
	cases.y_grid[1] = Y_un1_Ind;

	cases.x_prop[0] = p_Xln1;
	cases.x_prop[1] = p_Xun1;

	cases.y_prop[0] = p_Yln1;
	cases.y_prop[1] = p_Yun1;
}


// ------------------------------------------------------------------
// Additional stochasticity for reserves (default)
// ------------------------------------------------------------------
// Function to interpolate between grid points and to introduce additional stochasticity following Houston and McNamara 1999, e.g. to prevent grid effects or to include additional environmental stochasticity. 
void Forward::Stoch_HMcN_AddStochRes(double x_case, double y_case, FwStochXYPropResultStruct &cases)
{
	// Stochasticity for variable x (reserves)

	// Determine the first lower node:
	std::vector<double>::iterator X_un1_It  = std::upper_bound(_x_vec.begin(), _x_vec.end(), x_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int X_ln1_Ind = Chop((int) std::distance(_x_vec.begin(),X_un1_It)-1, 0, _x_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double X_ln1  = _x_vec[X_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other 3 surrounding nodes:
	int X_ln2_Ind = Chop(X_ln1_Ind - 1, 0, _x_cnt-1);						   // index of 2nd lower node
	int X_un1_Ind = Chop(X_ln1_Ind + 1, 0, _x_cnt-1);                          // index of 1st upper node
	int X_un2_Ind = Chop(X_ln1_Ind + 2, 0, _x_cnt-1);						   // index of 2nd upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Xun1_lc = (x_case-X_ln1)/_dx;									   // probability of 1st upper node, linear case
	double p_Xln1_lc = 1-p_Xun1_lc;											   // probability of 1st lower node, linear case

	// Control degree of stochasticity through stochfac alpha:
	double p_Xln2 = _stochfac_x * p_Xln1_lc;								   // probability of 2nd lower node
	double p_Xln1 = (1-2*_stochfac_x) * p_Xln1_lc + _stochfac_x * p_Xun1_lc;   // cumulative probability of 1st lower node
	double p_Xun1 = _stochfac_x * p_Xln1_lc + (1-2*_stochfac_x) * p_Xun1_lc;   // cumulative probability of 1st upper node
	double p_Xun2 = _stochfac_x * p_Xun1_lc;								   // cumulative probability of 2nd upper node - actually not needed, should be 1.0

	// Stochasticity for variable y (condition)

	// Determine the first upper node:
	std::vector<double>::iterator Y_un1_It  = std::upper_bound(_y_vec.begin(), _y_vec.end(), y_case) ;
	// iterator pointing to first element which doesn't compare less than y_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than y_case

	int Y_ln1_Ind = Chop((int) std::distance(_y_vec.begin(),Y_un1_It)-1, 0, _y_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account 
	// for std::distance (or that C starts from 0 respectively)

	double Y_ln1  = _y_vec[Y_ln1_Ind];										   // value of 1st lower node, within range of grid

	// Determine the indices of the other 3 surrounding nodes:
	int Y_un1_Ind = Chop(Y_ln1_Ind + 1, 0, _y_cnt-1);						   // index of first upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Yun1 = (y_case-Y_ln1)/_dy;                                        // probability of 1st upper node
	double p_Yln1 = 1-p_Yun1;                                                  // probability of 1st lower node

	// return results:
	cases.x_grid[0] = X_ln2_Ind;
	cases.x_grid[1] = X_ln1_Ind;
	cases.x_grid[2] = X_un1_Ind;
	cases.x_grid[3] = X_un2_Ind;

	cases.y_grid[0] = Y_ln1_Ind;
	cases.y_grid[1] = Y_un1_Ind;

	cases.x_prop[0] = p_Xln2;
	cases.x_prop[1] = p_Xln1;
	cases.x_prop[2] = p_Xun1;
	cases.x_prop[3] = p_Xun2;

	cases.y_prop[0] = p_Yln1;
	cases.y_prop[1] = p_Yun1;

}


// ------------------------------------------------------------------
// Additional stochasticity for health only
// ------------------------------------------------------------------
// Function to introduce stochasticity, e.g. to prevent grid effects or to include additional environmental stochasticity
void Forward::Stoch_HMcN_AddStochHealth(double x_case, double y_case, FwStochXYPropResultStruct &cases)
{
	// Stochasticity for variable x (reserves)

	// Determine the first lower node:
	std::vector<double>::iterator X_un1_It  = std::upper_bound(_x_vec.begin(), _x_vec.end(), x_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int X_ln1_Ind = Chop((int) std::distance(_x_vec.begin(),X_un1_It)-1, 0, _x_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double X_ln1  = _x_vec[X_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other surrounding node:
	int X_un1_Ind = Chop(X_ln1_Ind + 1, 0, _x_cnt-1);                          // index of 1st upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Xun1 = (x_case-X_ln1)/_dx;									   // probability of 1st upper node, linear case
	double p_Xln1 = 1-p_Xun1;											   // probability of 1st lower node, linear case

	// Stochasticity for variable y (condition)

	// Determine the first lower node:
	std::vector<double>::iterator Y_un1_It  = std::upper_bound(_y_vec.begin(), _y_vec.end(), y_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int Y_ln1_Ind = Chop((int) std::distance(_y_vec.begin(),Y_un1_It)-1, 0, _y_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double Y_ln1  = _y_vec[Y_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other 3 surrounding nodes:
	int Y_ln2_Ind = Chop(Y_ln1_Ind - 1, 0, _y_cnt-1);						   // index of 2nd lower node
	int Y_un1_Ind = Chop(Y_ln1_Ind + 1, 0, _y_cnt-1);                          // index of 1st upper node
	int Y_un2_Ind = Chop(Y_ln1_Ind + 2, 0, _y_cnt-1);						   // index of 2nd upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Yun1_lc = (y_case-Y_ln1)/_dy;									   // probability of 1st upper node, linear case
	double p_Yln1_lc = 1-p_Yun1_lc;											   // probability of 1st lower node, linear case

	// Control degree of stochasticity through stochfac alpha:
	double p_Yln2 = _stochfac_x * p_Yln1_lc;								   // probability of 2nd lower node
	double p_Yln1 = (1-2*_stochfac_x) * p_Yln1_lc + _stochfac_x * p_Yun1_lc;   // cumulative probability of 1st lower node
	double p_Yun1 = _stochfac_x * p_Yln1_lc + (1-2*_stochfac_x) * p_Yun1_lc;   // cumulative probability of 1st upper node
	double p_Yun2 = _stochfac_x * p_Yun1_lc;								   // cumulative probability of 2nd upper node - actually not needed, should be 1.0


	// return results:
	cases.x_grid[0] = X_ln1_Ind;
	cases.x_grid[1] = X_un1_Ind;

	cases.y_grid[0] = Y_ln2_Ind;
	cases.y_grid[1] = Y_ln1_Ind;
	cases.y_grid[2] = Y_un1_Ind;
	cases.y_grid[3] = Y_un2_Ind;

	cases.x_prop[0] = p_Xln1;
	cases.x_prop[1] = p_Xun1;

	cases.y_prop[0] = p_Yln2;
	cases.y_prop[1] = p_Yln1;
	cases.y_prop[2] = p_Yun1;
	cases.y_prop[3] = p_Yun2;

}

// ------------------------------------------------------------------
// Additional stochasticity for reserves and health
// ------------------------------------------------------------------
// Function to introduce stochasticity, e.g. to prevent grid effects or to include additional environmental stochasticity
void Forward::Stoch_HMcN_AddStochResHealth(double x_case, double y_case, FwStochXYPropResultStruct &cases)
{
	// Stochasticity for variable x (reserves)

	// Determine the first lower node:
	std::vector<double>::iterator X_un1_It  = std::upper_bound(_x_vec.begin(), _x_vec.end(), x_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int X_ln1_Ind = Chop((int) std::distance(_x_vec.begin(),X_un1_It)-1, 0, _x_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double X_ln1  = _x_vec[X_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other 3 surrounding nodes:
	int X_ln2_Ind = Chop(X_ln1_Ind - 1, 0, _x_cnt-1);						   // index of 2nd lower node
	int X_un1_Ind = Chop(X_ln1_Ind + 1, 0, _x_cnt-1);                          // index of 1st upper node
	int X_un2_Ind = Chop(X_ln1_Ind + 2, 0, _x_cnt-1);						   // index of 2nd upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Xun1_lc = (x_case-X_ln1)/_dx;									   // probability of 1st upper node, linear case
	double p_Xln1_lc = 1-p_Xun1_lc;											   // probability of 1st lower node, linear case

	// Control degree of stochasticity through stochfac alpha:
	double p_Xln2 = _stochfac_x * p_Xln1_lc;								   // probability of 2nd lower node
	double p_Xln1 = (1-2*_stochfac_x) * p_Xln1_lc + _stochfac_x * p_Xun1_lc;   // cumulative probability of 1st lower node
	double p_Xun1 = _stochfac_x * p_Xln1_lc + (1-2*_stochfac_x) * p_Xun1_lc;   // cumulative probability of 1st upper node
	double p_Xun2 = _stochfac_x * p_Xun1_lc;								   // cumulative probability of 2nd upper node - actually not needed, should be 1.0

	// Stochasticity for variable y (condition)

	// Determine the first lower node:
	std::vector<double>::iterator Y_un1_It  = std::upper_bound(_y_vec.begin(), _y_vec.end(), y_case);
	// iterator pointing to first element which doesn't compare less than x_case, i.e. to first upper node;
	// returns last element if all elements in the range compare less than x_case

	int Y_ln1_Ind = Chop((int) std::distance(_y_vec.begin(),Y_un1_It)-1, 0, _y_cnt-1 );
	// std::distance -> number of elements between first and last (including first and last)
	// x_vec.size()-1-1 <- -1 to account for transformation from un1 to ln1, and -1 to account for std::distance (or that C starts from 0 respectively)

	double Y_ln1  = _y_vec[Y_ln1_Ind];  // value of 1st lower node, within range of grid

	// Determine the indices of the other 3 surrounding nodes:
	int Y_ln2_Ind = Chop(Y_ln1_Ind - 1, 0, _y_cnt-1);						   // index of 2nd lower node
	int Y_un1_Ind = Chop(Y_ln1_Ind + 1, 0, _y_cnt-1);                          // index of 1st upper node
	int Y_un2_Ind = Chop(Y_ln1_Ind + 2, 0, _y_cnt-1);						   // index of 2nd upper node

	// Set probabilities that the value of these nodes is taken:
	double p_Yun1_lc = (y_case-Y_ln1)/_dy;									   // probability of 1st upper node, linear case
	double p_Yln1_lc = 1-p_Yun1_lc;											   // probability of 1st lower node, linear case

	// Control degree of stochasticity through stochfac alpha:
	double p_Yln2 = _stochfac_x * p_Yln1_lc;								   // probability of 2nd lower node
	double p_Yln1 = (1-2*_stochfac_x) * p_Yln1_lc + _stochfac_x * p_Yun1_lc;   // cumulative probability of 1st lower node
	double p_Yun1 = _stochfac_x * p_Yln1_lc + (1-2*_stochfac_x) * p_Yun1_lc;   // cumulative probability of 1st upper node
	double p_Yun2 = _stochfac_x * p_Yun1_lc;								   // cumulative probability of 2nd upper node - actually not needed, should be 1.0


	// return results:
	cases.x_grid[0] = X_ln2_Ind;
	cases.x_grid[1] = X_ln1_Ind;
	cases.x_grid[2] = X_un1_Ind;
	cases.x_grid[3] = X_un2_Ind;

	cases.y_grid[0] = Y_ln2_Ind;
	cases.y_grid[1] = Y_ln1_Ind;
	cases.y_grid[2] = Y_un1_Ind;
	cases.y_grid[3] = Y_un2_Ind;

	cases.x_prop[0] = p_Xln2;
	cases.x_prop[1] = p_Xln1;
	cases.x_prop[2] = p_Xun1;
	cases.x_prop[3] = p_Xun2;

	cases.y_prop[0] = p_Yln2;
	cases.y_prop[1] = p_Yln1;
	cases.y_prop[2] = p_Yun1;
	cases.y_prop[3] = p_Yun2;

}

//---------------------------------------

// generates pseudo-random number between 0 and 1 (uniform distribution)
double Forward::Rand01(void)
{
	return((double) rand() / ((double) RAND_MAX + 1));
}

// Test wether double variable contains valid number (trap NaN)
bool Forward::IsNumber(double x)
{
	return(x == x);
}

// Test wether double variable contains valid finite number (trap NaN, -/+ infinity)
bool Forward::IsFiniteNumber(double x) 
{
	return (x <= DBL_MAX && x >= -DBL_MAX); 
}

// calculate arithmetic mean from vector
double Forward::Mean(std::vector<double> vec)
{
	double sum_vec=0.0;
	int sum_cnt=0;
	for (unsigned int i=0;i<vec.size();i++)
	{
		if (IsFiniteNumber(vec[i])){
			sum_vec += vec[i]; 
			sum_cnt++;
		}		
	}

	return( (double) sum_vec/sum_cnt);
}

// calculate sum from double vector
double Forward::Sum(std::vector<double> vec)
{
	double sum_vec=0.0;
	for (unsigned int i=0;i<vec.size();i++)
	{
		sum_vec += vec[i];
	}

	return(sum_vec);
}

// calculate sum from integer vector
int Forward::Sum(std::vector<int> vec)
{
	int sum_vec=0;
	for (unsigned int i=0;i<vec.size();i++)
	{
		sum_vec += vec[i];
	}

	return(sum_vec);
}


//----------------------------------------

void Forward::Init(Settings *settings)
{
	//-------------------------------------
	// prepare parameters

	InitStateFuncs(settings, _decision->GetTheta() );

	_conv.fw_convergence = false;

	_user_init_start_pop = settings->GetUserInitStartPop();  
	_n_fw                = settings->GetNFW();
	_n_min_fw            = settings->GetNMinFW();
	_start_week_fw       = settings->GetStartWeekFW();
	_start_loc_fw        = settings->GetStartLocationFW();

	_t_cnt             = settings->GetTCnt();
	_n_brood           = settings->GetNBrood();
	_gamma_nest        = settings->GetGammaNest();

	_dx               = settings->GetDx();
	_dy               = settings->GetDy();

	_a_max            = settings->GetAMax();
	_e_cnt            = settings->GetECnt();
	_a_cnt            = settings->GetACnt();
	_o_cnt            = settings->GetOCnt();
	_s_cnt            = settings->GetSCnt();

	_m_migr           = settings->GetMMigr();

	_p_exp            = settings->GetPexp();
	_stochfac_x       = settings->GetStochFacX();


	_x_cnt            = settings->GetXCnt();
	double x_min      = settings->GetXMin();
	_x_vec.resize(_x_cnt);
	for (unsigned int x=0;x<_x_cnt;x++)
		_x_vec[x] = x_min+ x*_dx;    
	_x_indep          = settings->GetXindep();

	_y_cnt            = settings->GetYCnt();
	double y_min      = settings->GetYMin();
	_y_vec.resize(_y_cnt);
	for (unsigned int y=0;y<_y_cnt;y++)
		_y_vec[y] = y_min+ y*_dy;
	_y_indep          = settings->GetYindep();


	_crit             = settings->GetCrit();

	_x_max            = settings->GetXMax(); // Uses Forward._x_max  not StateFuncs._x_max
	_e_max            = settings->GetEMax(); // Uses Forward._e_max  not StateFuncs._e_max
	//---------------------------------------
	// allocate array for summary statistics

	// rows : weeks * years; columns: week, year, location, nr_ind, deaths_week, young_indep, prop_nocare, prop_start, prop_care, prop_migr, mean_res, mean_cond, p_death_disease, p_death_predation, p_death_res, p_death_cond, p_death_nestlings, brood_aband, dead_parent


	// Initialize the _stoch_hmcn_func function pointer to of the four grid interpolation functions
	// A call to Stoch_HMcN() then calls the selected function 
	// Initializes the FW_props iterator range 0.._xi_max / 0.._yi_max
	unsigned int idx = ((settings->GetStochAddReserves()?1:0) + (settings->GetStochAddHealth()?2:0));
	switch(idx)
	{
	case 0: 
		_stoch_hmcn_func = &Forward::Stoch_HMcN_AddStochNone;      
		_xi_max = 2;
		_yi_max = 2;
		break;
	case 1: 
		_stoch_hmcn_func = &Forward::Stoch_HMcN_AddStochRes;       
		_xi_max = 4;
		_yi_max = 2;
		break;
	case 2: 
		_stoch_hmcn_func = &Forward::Stoch_HMcN_AddStochHealth;    
		_xi_max = 2;
		_yi_max = 4;
		break;
	case 3: 
		_stoch_hmcn_func = &Forward::Stoch_HMcN_AddStochResHealth; 
		_xi_max = 4;
		_yi_max = 4;
		break;
	}

}


//================================================================================================
//
// Forward iteration following Toekoelyi et al. (fast but no individual tracking)
//
//================================================================================================


void Forward::CalcLambdaAndConvergence(NArray<double> &FW_props, NArray<double> &FW_old, FwConvResultStruct & result) {

	double lambda_worst = 1;		// Lambda value with highest difference to optimal 1.0
	double lambda_max_delta = 0;	// Highest deviation against optimal lambda of 1
	double lambda_average = 0;		// Average lambda over all state combinations
	int    state_count = 0;			// Count of state combinations used for average calcualtion
	int    notconv_count = 0;

	for (unsigned int x=0;x<_x_cnt;x++) {
		for (unsigned int y=0;y<_y_cnt;y++) {
			for (unsigned int e=0;e<_e_cnt;e++) {
				for (unsigned int a=0;a<_a_cnt;a++)	{
					for (unsigned int o=0; o<_o_cnt; o++) {
						for (unsigned int s=0; s<_s_cnt; s++) {
							for (unsigned int t=0;t<_t_cnt;t++)
							{
								double oldVal = FW_old(x,y,e,a,o,s,t);
								double newVal = FW_props(x,y,e,a,o,s,t);

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
	}

	lambda_average = lambda_average / state_count;

	result.lambda_fw_average = lambda_average;
    result.lambda_fw_worst   = lambda_worst;
	result.fw_notconv_count  = notconv_count;
	result.fw_state_count    = state_count;
	result.fw_convergence    = lambda_max_delta < _crit;
	
	// For comparison (lambda calculated for particular state)
	double oldVal     = FW_old(_x_cnt-1,_y_cnt-1,_e_cnt-1,0,0,0,_t_cnt-1);
	double newVal     = FW_props(_x_cnt-1,_y_cnt-1,_e_cnt-1,0,0,0,_t_cnt-1);
	result.lambda_fw_state  = (double) newVal/oldVal;;
}

// ------------------------------------------------------------------------------------------------
// Computation of probability distribution for all state combinations
// ------------------------------------------------------------------------------------------------


double Forward::ComputePopulationDynamics(Settings *settings) {

	if (!_decision) {
		printf("Forward::ComputePopulationDynamics() ERROR: '_decision' not initialized\n");
	}

	printf("Forward::ComputePopulationDynamics() computing...\n");


	//-------------------------------------
	// prepare parameters
	Init(settings);


	//---------------------------------------
	// allocate state array for storing proportions

	NArray<double> FW_props_renorm, FW_props, FW_old;
	FW_props.Init(_x_cnt, _y_cnt, _e_cnt, _a_cnt, _o_cnt, _s_cnt, _t_cnt);
	FW_old.Init(_x_cnt, _y_cnt, _e_cnt, _a_cnt, _o_cnt, _s_cnt, _t_cnt);
	FW_props_renorm.Init(_x_cnt, _y_cnt, _e_cnt, _a_cnt, _o_cnt, _s_cnt, _t_cnt);

	double lambda=0.0;
	bool convergence=false;
	double lambda_state=0.0;

	// initiate starting cohort
	FwStochXYPropResultStruct cases;
	Stoch_HMcN(_x_indep, _y_indep, cases);

	if ( _user_init_start_pop) {	 
		sprintf_s(_filename_fw_pd , "%s_populationdynamics_FW.bin", settings->GetFilePrefixFW());
		if ( !LoadFwFromFile(_filename_fw_pd) ) {
			printf("Error loading forward results");
			exit(-1);
		}
		else {			
			printf("Forward::ComputePopulationDynamics() loaded initial start population from file\n");
			FW_props = _FW_props; 
		}
	}	
	else {
		for (unsigned int xi=0; xi<_xi_max; xi++) {
			for (unsigned int yi=0; yi<_yi_max; yi++) {
				FW_props(cases.x_grid[xi],cases.y_grid[yi],0,0,_start_loc_fw-1,0,_start_week_fw) += cases.x_prop[xi]*cases.y_prop[yi];
			}}

	} 
	//-------------------------------------------
	printf("\n");

	double lambda_old_fw = 0;
	double dlambda_fw = 999;
	unsigned int year=0; 
	unsigned int t_start_week = _start_week_fw;

	if (_user_init_start_pop)
		t_start_week = 0;	


	bool errorInSimulation = false; // Quick exit of loops due to 'no more living' or 'exponential growth'

	while ((!convergence && year<_n_fw) || year<_n_min_fw)
	{	

		FW_old = FW_props;

		for (unsigned int t=t_start_week;t<_t_cnt;t++) {

				// reset t+1 values to zero 
				for (unsigned int x=0;x<_x_cnt;x++) {
					for (unsigned int y=0;y<_y_cnt;y++) {
						for (unsigned int e=0;e<_e_cnt;e++) {
							for (unsigned int a=0;a<_a_cnt;a++) {
								for (unsigned int o=0;o<_o_cnt;o++) {
									for (unsigned int s=0;s<_s_cnt;s++) {
										FW_props(x,y,e,a,o,s,(t+1)%_t_cnt) = 0.0;
									}}}}}}


				//------------------------------------------------
				// add newly independent brood

				for (unsigned int x=1;x<_x_cnt;x++) {
					// for (unsigned int y=1;y<_y_cnt;y++) {  // original
					for (unsigned int y=0;y<_y_cnt;y++) {
						for (unsigned int e=0;e<_e_cnt;e++) {
							for (unsigned int o=0;o<_o_cnt;o++) {

								if (FW_props(x,y,e,_a_cnt-1,o,0,t) > 0.0) {

									Stoch_HMcN(_x_indep, _y_indep, cases);

									for (unsigned int xi=0; xi<_xi_max; xi++) {
										for (unsigned int yi=0; yi<_yi_max; yi++) {
											FW_props(cases.x_grid[xi],cases.y_grid[yi],0,0,o,0,t) += _n_brood * FW_props(x,y,e,_a_cnt-1,o,0,t) * cases.x_prop[xi] * cases.y_prop[yi];
										}}

								}}}}}


				//------------------------------------------------
				// optimal behaviour
				for (unsigned int x=1;x<_x_cnt;x++) {
					// for (unsigned int y=1;y<_y_cnt;y++) {  // original
					for (unsigned int y=0;y<_y_cnt;y++) {
						for (unsigned int e=0;e<_e_cnt;e++) {
							for (unsigned int a=0;a<_a_cnt;a++) {
								for (unsigned int o=0;o<_o_cnt;o++) {
									for (unsigned int s=0;s<_s_cnt;s++) {

										if (FW_props(x,y,e,a,o,s,t) > 0.0) {

											// optimal foraging intensity and optimal strategy
											double u_opt = _decision->GetF_u(x, y, e, a, o, s, t);
											char   strat = _decision->GetF_strat(x, y, e, a, o, s, t);

											double die_pred, die_dis;
											// predation related mortality - note that this mortality acts only on strategies != migrate
											if (strat!='m') {
												die_pred = M(o,u_opt,_x_vec[x],_x_max);
											}
											else die_pred = M_mig(_x_vec[x]); //die_pred = _m_migr;
											// disease related mortality
											die_dis = D(_y_vec[y]);

											// note that mortality due to depletion of reserves or condition is modelled implicitly and can be extracted from the x=0 and y=0 columns of FW_props

											//---------------------------------------
											// no care

											if (strat == 'n') {
												Stoch_HMcN(X_nc(_x_vec[x], e, a, o, u_opt, t), Y_ns(_x_vec[x],_y_vec[y], u_opt, t), cases);

												for (unsigned int xi=0; xi<_xi_max; xi++) {
													for (unsigned int yi=0; yi<_yi_max; yi++) {

														FW_props(cases.x_grid[xi],cases.y_grid[yi],Chop(e+1,0,(_e_cnt-1)),0,o,s,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * _p_exp;                      
														FW_props(cases.x_grid[xi],cases.y_grid[yi],e,0,o,s,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * (1-_p_exp);
													}}

											}  else // end loop over no_care

												//---------------------------------------
												//  start brood

												if (strat == 's') {
													Stoch_HMcN(X_s(_x_vec[x], e, a, o, u_opt, t), Y_s(_x_vec[x],_y_vec[y], u_opt, t), cases);

													for (unsigned int xi=0; xi<_xi_max; xi++) {
														for (unsigned int yi=0; yi<_yi_max; yi++) {

															FW_props(cases.x_grid[xi],cases.y_grid[yi],Chop(e+1,0,(_e_cnt-1)),1,o,s,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * _p_exp;

															FW_props(cases.x_grid[xi],cases.y_grid[yi],e,1,o,s,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * (1-_p_exp);

														}}

												} else // end loop over start brood

													//---------------------------------------
													//  care for brood

													if (strat == 'c') {

														Stoch_HMcN(X_c(_x_vec[x], e, a, o, u_opt, t), Y_ns(_x_vec[x], _y_vec[y], u_opt, t), cases);

														for (unsigned int xi=0; xi<_xi_max; xi++) {
															for (unsigned int yi=0; yi<_yi_max; yi++) {

																FW_props(cases.x_grid[xi],cases.y_grid[yi],Chop(e+1,0,(_e_cnt-1)),a+1,o,s,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * _p_exp;

																FW_props(cases.x_grid[xi],cases.y_grid[yi],e,a+1,o,s,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * (1-_p_exp);

															}}

													} else // end loop over care for brood

														//--------------------------------------
														// migrate

														if (strat == 'm') {
															Stoch_HMcN(X_m(_x_vec[x], e, 0, o, s, u_opt, t), Y_m(_x_vec[x], _y_vec[y], o, s, u_opt, t), cases);

															if (s < (_s_cnt-1)) {
																for (unsigned int xi=0; xi<_xi_max; xi++) {
																	for (unsigned int yi=0; yi<_yi_max; yi++) {

																		FW_props(cases.x_grid[xi],cases.y_grid[yi],Chop(e+1,0,(_e_cnt-1)),0,o,s+1,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * _p_exp;

																		FW_props(cases.x_grid[xi],cases.y_grid[yi],e,0,o,s+1,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * (1-_p_exp);

																	}}
															} else {    // s = (s_cnt-1) -> last week of migration -> at other location and not migrating anymore in next week
																for (unsigned int xi=0; xi<_xi_max; xi++) {
																	for (unsigned int yi=0; yi<_yi_max; yi++) {

																		FW_props(cases.x_grid[xi],cases.y_grid[yi],Chop(e+1,0,(_e_cnt-1)),0,(o+1)%_o_cnt,0,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * _p_exp;

																		FW_props(cases.x_grid[xi],cases.y_grid[yi],e,0,(o+1)%_o_cnt,0,(t+1)%_t_cnt) += FW_props(x,y,e,a,o,s,t) * cases.x_prop[xi] * cases.y_prop[yi] * (1-die_pred) * (1-die_dis) * (1-_p_exp);

																	}}
															}

														} // end loop over migration

										}

									}}}}}} // end loop over states

			} // end loop over weeks

		// The years following the first one (which starts at t_start_week) will start at week zero:
		t_start_week = 0;

		// calculate lambda & check convergence

		CalcLambdaAndConvergence(FW_props, FW_old, _conv);

		lambda       = _conv.lambda_fw_average;
		convergence  = _conv.fw_convergence;
		lambda_state = _conv.lambda_fw_state;
		if (convergence) {
			_conv.fw_year_conv = year;
		}			

//		printf("=========== Forward iteration %2d done,  Lambda Fwd = %.4f\n\n", year, lambda_state);		

		printf("=========== Forward iteration %2d  done ===========\n",year);	 
		printf("........... Lambda:  average = %f   specific state = %f\n",_conv.lambda_fw_average,_conv.lambda_fw_state);
		printf("........... Lambda:  worst   = %f   not converged %d of %d \n", _conv.lambda_fw_worst,_conv.fw_notconv_count,_conv.fw_state_count );
		printf("\n");

		// store/overwrite results
		_FW_props = FW_props;		

		if (_user_init_start_pop) {
			sprintf_s(_filename_fw_pd_year , "%s_populationdynamics_FW_%2d.bin", settings->GetFilePrefixFW(),year);
			SavePopulationDynamics(_filename_fw_pd_year);
		}

		dlambda_fw    = fabs(lambda-lambda_old_fw);
		lambda_old_fw = lambda;
		year++;

		// } // end loop over years
	} // end while loop over years and dlambda    

	return (lambda);

}

