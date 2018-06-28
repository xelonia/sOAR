/**
* \file Backward.h
* \brief Declaration of class Backward
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

#ifndef __OAR_CPP__Backward__
#define __OAR_CPP__Backward__

#include <stdio.h>

#include "..\soar_support_lib\Nanotimer.h"
#include "..\soar_support_lib\NArray.h"

#include "StateFuncs.h"
#include "Optimizer.h"

// Forward declarations of external classes
class Decision;
class Settings;

/**
 * \ingroup SoarLib
 * \brief Backward iteration for computing the optimal behavioural strategy
 * <HR />
 *
 * <H2 class="groupheader">Include, Inheritance and Collaboration</H2>
 */
class Backward : public StateFuncs {
    
private:

#ifdef GENERATE_DOXYGEN_DOC
	Settings * doxygen;   ///< DOXYGEN: The settings class contains all parameters for the simulation
#endif

	/// \name Read only configuration parameters retrieved from Settings 
	/// @{ 
    int				_n;	          ///< Maximum number of periods in backward iteration (eg years)
	int             _n_min;	      ///< Minimum number of periods in backward iteration (eg years)
    int				_n_brood;	  ///< Number of offspring 
    double			_gamma_nest;  ///< Reserve costs of care for young
    double			_crit;		  ///< Convergence level

	unsigned int	_e_cnt;		  ///< Number of experiences
    unsigned int	_a_cnt;		  ///< Number of ages
	unsigned int	_o_cnt;       ///< Number of locations
	unsigned int	_s_cnt;		  ///< Number of stop over locations plus 1
    unsigned int    _t_cnt;       ///< Decision epochs per period (eg number of timesteps per year)

    int				_a_max;       ///< Age of independance

	double			 _min_x;		///< Reserves min 
	double			 _min_y;		///< Health min
    double			 _dx;			///< Reserved delta step in x
    double			 _dy;			///< Health delta step in y
	double			 _inverse_dx;   ///< 1.0/_dx, used for optimization since multiplication is faster than division
	double			 _inverse_dy;	///< 1.0/_dy, used for optimization since multiplication is faster than division
    unsigned int	 _x_cnt;		///< Number of reserves grid points in x
    unsigned int	 _y_cnt;		///< Number of health grid points in y
    double			 _stochfac_x;	///< Degree of potentially added stochasticity retrieved from Settings

    double			_x_indep;     ///< Start x value for independent young
    double			_y_indep;     ///< Start y value for independent young
    
    double			_p_exp;       ///< Probability of growth of experience per decision epoch

	int				_migr_dur;    ///< Duration of migration
	///@} End of group started by \name
        

	/// \name Local variables
	/// @{ 
	unsigned int	_loc_idx;	  ///< Current location index in Compute()

	NArray<double>	_x_vec;		 ///< Read only array containing the x value from x_min to x_max for each gridstep
	NArray<double>	_y_vec;		 ///< Read only array containing the y value from y_min to y_max for each gridstep

	NArray<double>	 _f_curr;	///< 2D Array for current value
	NArray<double>   _f_next;	///< 2D Array for current value

#ifdef BW_TIMING
    NanoTimer _timer_week;		///< Timer to time the calculations for one simulated week	
#endif

    Decision  * _decision;		///< Reference to storage for computed strategy
	Optimizer   _optimizer;     ///< Instance used for optimization

	/**
	 * \ingroup SoarLib
	 * \brief Result structure for the Stoch_HMcN grid interpoation functions
	 */
    struct BwStochResultStruct {
        double curr_approx;
        double next_approx;
    };

	/// Function pointer to one of the four grid interpolation variants
	void (Backward::*_stoch_hmcn_func)(double x_case, double y_case, BwStochResultStruct &result);
	///@} End of group started by \name


	/// \name Variants for interpolation between grid points
	/// @{ 
	  /**
	   * Computes linear interpolation between grid points without adding further stochasticity
	   * @param x_case  Input in x dimension
	   * @param y_case  Input in y dimension
	   * @param result  Output structure containing stochasticity approximations
	   */
	void Stoch_HMcN_AddStochNone(double x_case, double y_case, BwStochResultStruct &result);

	  /**
	   * Computes linear interpolation between grid points with further stochasticity added for reserves variable
	   * @param x_case  Input in x dimension
	   * @param y_case  Input in y dimension
	   * @param result  Output structure containing stochasticity approximations
	   */
	void Stoch_HMcN_AddStochRes(double x_case, double y_case, BwStochResultStruct &result);

	  /**
	   * Computes linear interpolation between grid points with further stochasticity added for health variable
	   * @param x_case  Input in x dimension
	   * @param y_case  Input in y dimension
	   * @param result  Output structure containing stochasticity approximations
	   */
	void Stoch_HMcN_AddStochHealth(double x_case, double y_case, BwStochResultStruct &result);

	  /**
	   * Computes linear interpolation between grid points with further stochasticity added for reserves and health variable
	   * @param x_case  Input in x dimension
	   * @param y_case  Input in y dimension
	   * @param result  Output structure containing stochasticity approximations
	   */
	void Stoch_HMcN_AddStochResHealth(double x_case, double y_case, BwStochResultStruct &result);

	  /**
	   * Calls the active variant for linear interpolation between grid points (Inline function)
	   * @param x_case  Input in x dimension
	   * @param y_case  Input in y dimension
	   * @param result  Output structure containing stochasticity approximations
	   */
	void Stoch_HMcN(double x_case, double y_case, BwStochResultStruct &result)
	{
		(this->*_stoch_hmcn_func)(x_case, y_case, result);
	}
	///@} End of group started by \name


	/// \name Payoff functions
	/// @{ 
	double H_m(double x, double y, int e, int a, int o, int s, int t, double u);
    double H_nmg(double x, double y, double u, int o, double f_currexp, double f_nextexp);
    double H_mg(double x, double y, double f_currexp, double f_nextexp);
	///@} End of group started by \name
   
	void UpdateFCurrFNext(int e, int a, int o, int s, int t);

	/**
	 * \ingroup SoarLib
	 * \brief Result structure for the CalcLambdaAndConvergence() function
	 */
    struct BwConvResultStruct {
        double lambda_bw_average;  ///< Average lambda over all state combinations
		double lambda_bw_state;    ///< Lambda of a specific state combination (_x_cnt-1,_y_cnt-1,_e_cnt-1,0,0,0,_t_cnt-1)
		double lambda_bw_worst;	   ///< Lambda value with highest difference to optimal 1.0
		int    bw_notconv_count;   ///< Number of not converged state combinations
		int    bw_state_count;     ///< Number of state combinations
		bool   bw_convergence;	   ///< Indicator of convergence
    };

	void CalcLambdaAndConvergence(NArray<double> &f_old ,double lambda_old, BwConvResultStruct & result);


	/**
	 * \ingroup SoarLib
	 * \brief Typedef for the functions called by the Optimizer(), for instance H_nc(), H_s() and H_c().
	 */
	typedef double (Backward::*BwPaybackFunc)(double,double,int,int,int,int,int,double);


	/**
	 * \ingroup SoarLib
	 * \brief Callback class for Optimizer used in ComputeHNoCare(), ComputeHStart() and ComputeHCare().
	 */
	class BwOptimizerFunc : public Optimizer::OptimizerFunc
	{
	private:
		double _x,_y;
		int _e,_a,_o,_s,_t;
		Backward      *_bw;
		BwPaybackFunc _func;
	public:
		BwOptimizerFunc(double x, double y, int e, int a, int o, int s, int t, Backward *bw, BwPaybackFunc func) 
			: _x(x), _y(y), _e(e), _a(a), _o(o), _s(s), _t(t), _bw(bw), _func(func) 
		{}

		double operator()(double val) { return - (_bw->*_func)(_x,_y,_e,_a,_o,_s,_t,val); }
	};

	/**
	 * \ingroup SoarLib
	 * \brief Result structure for the ComputeHNoCare(),ComputeHStart(),ComputeHCare and ComputeHMigrate()  functions
	 */
	struct BwOptResultStruct {
		double H;
		double u;
		char   s;
	};

	void ComputeHNoCare( int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result); 
	void ComputeHStart(  int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result); 
	void ComputeHCare(   int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result); 
	void ComputeHMigrate(int res, int cond, int ex, int age, int loc, int s, int week,BwOptResultStruct &result); 
	
	bool InitBackward(Settings *set, double theta);
public:
    Backward();
    ~Backward();
    
    void SetDecision(Decision *dec);
            
    // Compute() returns the lambda
    double Compute(Settings *set, double theta);

	// payoff functions - Called via optimizer
    double H_nc (double x, double y, int e, int a, int o, int s, int t, double u);
    double H_s (double x, double y, int e, int a, int o, int s, int t, double u);
    double H_c (double x, double y, int e, int a, int o, int s, int t, double u);
};


#endif /* defined(__OAR_CPP__Backward__) */
