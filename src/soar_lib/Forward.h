/**
* \file Forward.h
* \brief Declaration of class Forward
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

#ifndef __OAR_CPP__Forward__
#define __OAR_CPP__Forward__

#include <list>
#include <vector>

#include "StateFuncs.h"
#include "..\soar_support_lib\NArray.h"
#include "Settings.h"

// Forward declarations of external classes
class Decision;
class Individual;

/**
 * \ingroup SoarLib
 * \brief Forwards simulation of animal behavior
 * <HR />
 *
 * <H2 class="groupheader">Include, Inheritance and Collaboration</H2>
 */
class Forward : public StateFuncs {
private:
    Decision *		_decision;
       
    unsigned int	_n_fw;
	unsigned int    _n_min_fw;
	unsigned int	_start_week_fw;
	unsigned int	_start_loc_fw;
    unsigned int    _o_cnt;
   
    unsigned int	_t_cnt;
    unsigned int	_n_brood;
    double			_gamma_nest;
    
    double			_dx;
    double			_dy;
    double			_x_indep;
    double			_y_indep;
    
    double			_x_max;    
	std::vector<double> _x_vec;
    std::vector<double> _y_vec;
    
    int				_a_max;
    int				_e_max;

    unsigned int	_x_cnt;
    unsigned int	_y_cnt;
    unsigned int    _e_cnt;
    unsigned int    _a_cnt;
    unsigned int    _s_cnt;

    double			_stochfac_x;    
    
    double			_p_exp;
    double          _m_migr;

	double          _dres_migr_act;
	double          _dres_migr_pas;
	double          _dcond_migr_act;
	double          _dcond_migr_pas;
    
    double          _crit;        

	unsigned int    _xi_max;
	unsigned int    _yi_max;

#ifdef GENERATE_DOXYGEN_DOC
    Individual * _population;
#else
    std::list<Individual> _population;
#endif	
    NArray<double>	_fw_summary;
	unsigned int    _l_fw;
    unsigned int	_cohortsize_fw;
    void PopInit(int nr_ind, int o, int t, int year);   
    void   PopToFile(char *filename);   ///< Save population list into a single binary file   

	int    _yearFw;												
	void   SetYearFw(int y)		{	_yearFw = y;	 }	
	int    GetYearFw()			{	return _yearFw;	 }	

	/**
	 * \ingroup SoarLib
	 * \brief Result structure for the Stoch_HMcN_xy() function
	 */
	struct FwStochXYResultStruct {
        int x_grid;
        int y_grid;
    };
    
	/**
	 * \ingroup SoarLib
	 * \brief Result structure for the Stoch_HMcN grid interpoation functions
	 */
    struct FwStochXYPropResultStruct {
        int x_grid[4];
        int y_grid[4];	  
        double x_prop[4];
        double y_prop[4]; 
    };
    
	/**
	 * \ingroup SoarLib
	 * \brief Result structure for the CalcLambdaAndConvergence() and CalcLambdaAndConvergence_Toekoelyi() functions
	 */
    struct FwConvResultStruct {		
        double lambda_fw_average;  // Average lambda over all state combinations
		double lambda_fw_state;    // Lambda of a specific state combination (_x_cnt-1,_y_cnt-1,_e_cnt-1,0,0,0,_t_cnt-1)
		double lambda_fw_worst;	   // Lambda value with highest difference to optimal 1.0
		int    fw_notconv_count;   // Number of not converged state combinations
		int    fw_state_count;     // Number of state combinations
		bool   fw_convergence;	   // Indicator of convergence
		int    fw_year_conv;	   // Year of convergence, or number of cycle, respectively		
    };

	NArray<double>     _FW_props;	
	FwConvResultStruct _conv;

	void Stoch_HMcN_AddStochNone (double x_case, double y_case, FwStochXYPropResultStruct &cases);
	void Stoch_HMcN_AddStochRes (double x_case, double y_case, FwStochXYPropResultStruct &cases);
	void Stoch_HMcN_AddStochHealth (double x_case, double y_case, FwStochXYPropResultStruct &cases);
	void Stoch_HMcN_AddStochResHealth (double x_case, double y_case, FwStochXYPropResultStruct &cases);

	/// Function pointer to one of the four grid interpolation variants
	void (Forward::*_stoch_hmcn_func)(double x_case, double y_case, FwStochXYPropResultStruct &result);

	  /**
	   * Calls the active variant for linear interpolation between grid points (Inline function)
	   * @param x_case  Input in x dimension
	   * @param y_case  Input in y dimension
	   * @param result  Output structure containing stochasticity approximations
	   */
	void Stoch_HMcN(double x_case, double y_case, FwStochXYPropResultStruct &result)
	{
		(this->*_stoch_hmcn_func)(x_case, y_case, result);
	}

    void CalcLambdaAndConvergence(NArray<double> &FW_props, NArray<double> &FW_old ,FwConvResultStruct & result);
	void CalcLambdaAndConvergence_Toekoelyi(NArray<double> &FW_props, NArray<double> &FW_old ,FwConvResultStruct & result);
        
    double Rand01(void);
	bool IsNumber(double);
	bool IsFiniteNumber(double);
    double Mean(std::vector<double> vec);
    double Sum(std::vector<double> vec);
    int Sum(std::vector<int> vec);

	void Init(Settings *settings);

public:
    Forward();
    ~Forward();
    
    void SetDecision(Decision *dec);

    double ComputePopulationDynamics(Settings *settings);   
    void   SavePopulationDynamics(char *filename); 

};


#endif /* defined(__OAR_CPP__Forward__) */
