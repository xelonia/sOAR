/**
* \file StateFuncs.h
* \brief Declaration of class StateFuncs
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

#ifndef STATE_FUNCS_H
#define STATE_FUNCS_H

#include "..\soar_support_lib\FuncType.h"
#include "..\soar_support_lib\NArray.h"

class Settings;

/**
 * \mainpage
 * The sOAR Optimal Annual Routine library...
 * <BR />
 * <DIV class="groupHeader">Overview</DIV>
 * The main classes are
 * - Settings containing the input parameters for both Backward and Forward simulation
 * - Backward searching backward in time for each animal state to find the
 *   the optimal state to enter the given state.
 * - StateFuncs defining the functions used for state changing. Some of these functions
 *   use the FuncType class to allow user defined functions
 * - Decision contains the optimal strategy computed by backward
 * - Forward runs the simulation from a starting population forward in time to check the strategy 
 *   using the Settings and Decision computed by Backward.
 */

/**
 * \ingroup SoarLib
 * \brief Simulation functions used to compute the state changes 
 * <HR />
 *
 * <H2 class="groupheader">Include and Inheritance</H2>
 */
class StateFuncs {
private:
	/// \name Read only configuration parameters retrieved from Settings 
	/// @{ 
	double        _x_min;	///< Reserves min 
    double        _y_min;	///< Health level min
	
	double        _x_max;	///< Reserves max 
    double        _y_max;	///< Health level max

    unsigned int  _e_max;	///< Maximum experience
	
    unsigned int  _t_max;	///< Decision epochs per period (eg number of timesteps per year) - named TCnt in Settings

    double _theta;			///< Factor to implicitly capture density dependence action on offspring
    NArray<double> _eps;	///< Average food supply
	NArray<double> _a_bar;	///< Food seasonality


	double _m_migr;			///< Predation risk during migration per decision epoch
	double _dres_migr_act;	///< Reserve costs of active flight
	double _dres_migr_pas;	///< Reserve costs of passive flight
	double _dcond_migr_act;	///< Health costs of active flight
	double _dcond_migr_pas; ///< Health costs of passive flight

    double _delta_res_start;	///< Reserve costs of incubation
    double _delta_cond_start;	///< Reserve costs of care for young
	
	double _c_bmr;				///< Basal metabolic rate

	double _p_active_flight_const;	  ///< Probability of active flight (if no CSV array is given)
	NArray<double> _p_active_flight;  ///< Probability of active flight array (time, location)
	NArray<double> _env_food_supply;  ///< Food supply per decision epoch and location

	FuncType _c_bmr_x_func;		///< Reserve dependency of BMR
	FuncType _c_u_func;			///< Activity dependency of metabolism
	FuncType _c_x_func;			///< Reserve dependency of metabolism
	FuncType _m_u_func[2];		///< Activity dependency of predation per location
	FuncType _m_x_func[2];		///< Reserves dependency of predation per location
	FuncType _m_migr_x_func;	///< Reserve dependency of predation during migration
	FuncType _alpha_func;		///< Metabolism dependency of health
	FuncType _d_func;			///< Disease risk
	FuncType _migr_act_x_func;	///< Reserve dependency of active flight
	FuncType _migr_pas_x_func;	///< Reserve dependency of passive flight

	///@} End of group started by \name

	/// \name Local variables
	/// @{ 	
    double *_gamma_brood_ind;	///< Array containing gamma incub or gamma nest for each age
	///@} End of group started by \name

protected:


    // Helper
    double Chop (double x, double minx, double maxx);
    int    Chop(int x, int minx, int maxx);
    double Env(int o, int t);
    double Gamma(int e, int o, double u, int t);
    double GammaBrood(int a);
    double U_crit(int e, int a, int o, int t);
    double C_u (double x, double _x_max, double u);
    double Alpha (double c);
	double D (double y);
	double P_active_flight(int o, int s, int t);
	double M (int o, double u, double x, double _x_max);
	double M_mig (double x);
	double S (double x, double y, int o, double u);
    double S_mig (double x, double y);
    
    // state variable functions
    double X_nc (double x, int e, int a, int o, double u, int t);
    double X_c (double x, int e, int a, int o, double u, int t);
    double X_s (double x, int e, int a, int o, double u, int t);
	double X_m (double x, int e, int a, int o, int s, double u, int t);
    double Y_s (double x, double y, double u, int t);
    double Y_ns (double x, double y, double u, int t);
	double Y_m (double x, double y, int o, int s, double u, int t);    

	void InitStateFuncs(Settings *settings, double theta);
public:
	StateFuncs();
	~StateFuncs();

	void PrintFuncs();
};

#endif // STATE_FUNCS_H