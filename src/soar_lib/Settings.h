/**
* \file Settings.h
* \brief Declaration of class Settings
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "..\soar_support_lib\FuncType.h"
#include "..\soar_support_lib\NArray.h"
#include "..\soar_support_lib\ParamManager.h"

//#define SUPPORT_FW_MONTE_CARLO

/**
* \defgroup SoarLib sOAR_Lib
* This module contains the classes that define the sOAR Optimal Annual Routine library.
*/

/**
 * \ingroup SoarLib
 * \brief Contains all the simulation parameters used in forward and backward optimization
 * <HR />
 *
 * <H2 class="groupheader">Include and Collaboration</H2>
 */
class Settings {
private:
	/// \name Parameters not stored in the config files
	/// @{ 
	ParamManager _pm;			   ///< Manages binary and ascii file operations for all paramaters
	unsigned int _n_min;		   ///< Minimum number of periods in backward iteration (eg years)
	unsigned int _n_min_fw;        ///< Minimum number of iterations in forward computation
	///@} End of group started by \name
	
	/// \name Parameters stored in binary and ascii config, managed by the ParamManager 
	/// @{ 
	bool     _run_forward;         ///< Run backward simulation
	bool     _run_backward;        ///< Run forward simulation
	bool     _enable_migration;    ///< Enable migration option
	bool     _enable_health_dim;   ///< Enable health dimension
	bool     _calibrate_theta;     ///< Backward calibrate theta
	double   _calibrate_theta_min; ///< Minimum theta for calibration

	unsigned int _n;      ///< Maximum number of periods in backward iteration (eg years)
    unsigned int _t_cnt;  ///< Decision epochs per period (eg number of timesteps per year)
	unsigned int _grid_x; ///< Reserves subdivisions
	unsigned int _grid_y; ///< Health   subdivisions

	double _x_min;		  ///< Reserves min 
	double _x_max;		  ///< Reserves max
	double _y_min;        ///< Health level min
	double _y_max;        ///< Health level max
    
    unsigned int _e_max;   ///< Maximum experience
    unsigned int _a_max;   ///< Age of independance

	unsigned int _o_cnt;   ///< Number of locations

    double _theta;         ///< Factor to implicitly capture density dependence action on offspring
	double _p_exp;         ///< Probability of growth of experience per decision epoch
	
	unsigned int  _incubation; ///< Duration of incubation
	double _gamma_incub;       ///< Reserve costs of incubation
	unsigned int  _n_brood;    ///< Number of offspring
	double _gamma_nest;        ///< Reserve costs of care for young

	double _fit_indep;      ///< Start fitness for independent young

	double _delta_res_start;   ///< Reserve costs of incubation
	double _delta_cond_start;  ///< Reserve costs of care for young

	double _bg_disease;	 ///< Background mortality due to disease
		

	NArray<double> _eps;             ///< Average food supply
	NArray<double> _a_bar;           ///< Food seasonality
	NArray<double> _env_food_supply; ///< Food supply per decision epoch and location

	unsigned int    _migr_dur;    ///< Duration of migration
	double _m_migr;               ///< Predation risk during migration per decision epoch
	double _dres_migr_act;        ///< Reserve costs of active flight
	double _dres_migr_pas;        ///< Reserve costs of passive flight
	double _dcond_migr_act;       ///< Health costs of active flight
	double _dcond_migr_pas;       ///< Health costs of passive flight

	double _p_active_flight_const;  ///< Probability of active flight  (if no CSV array is given)

	NArray<double> _p_active_flight; ///< Probability of active flight array (time, location)

	double _crit;                    ///< Convergence level
	bool   _stoch_add_reserves;		 ///< Add stochasticity reserves to grid interpolation
	bool   _stoch_add_health;		 ///< Add stochasticity health to grid interpolation
	double _stochfac_x;              ///< Stochasticity factor

	double _c_bmr;                    ///< Basal metabolic rate 
	FuncType _c_bmr_x_func;           ///< Reserve dependency of BMR
	FuncType _c_u_func;               ///< Activity dependency of metabolism
	FuncType _c_x_func;               ///< Reserve dependency of metabolism

	FuncType _m_u_func[2];            ///< Activity dependency of predation per location
	FuncType _m_x_func[2];            ///< Reserves dependency of predation per location

	FuncType _m_migr_x_func;          ///< Reserve dependency of predation during migration

	FuncType _alpha_func;             ///< Metabolism dependency of health


	FuncType _migr_act_x_func;        ///< Reserve dependency of active flight
	FuncType _migr_pas_x_func;        ///< Reserve dependency of passive flight

	char _file_prefix[512];           ///< File prefix for output of backward iteration


    unsigned int _n_fw;               ///< Maximum number of iteration years in forward computation
    unsigned int _start_week_fw;      ///< Start epoch for forward simulation
	unsigned int _start_loc_fw;       ///< Initial location for forward simulation
	char         _file_prefix_fw[512];///< File prefix for output of forward  iteration

	///@} End of group started by \name

	/// \name Parameters calculated from the config
	/// @{ 
	double _dx;			   ///< Reserved delta step in x, computed from grid 
    double _dy;			   ///< Health delta step   in y, computed from grid 
    unsigned int _x_cnt;   ///< Number of reserves grid points in x, computed from grid size
    unsigned int _y_cnt;   ///< Number of health   grid points in y, computed from grid size
    double _x_indep;		///< Start x value for independent young, computed from _fit_indep and grid size
    double _y_indep;		///< Start y value for independent young, computed from _fit_indep and grid size

    unsigned int _a_cnt;   ///< Number of ages, computed from age of independance and incubation 
    unsigned int _e_cnt;   ///< Number of experiences, computed from maximum experience
	unsigned int _s_cnt;   ///< Number of stop over locations plus 1, computed from migrDur

	FuncType _d_func;      ///< Disease risk, computed from background mortality associated to disease and metabolism dependency of health.

	///@} End of group started by \name


	void ComputeDependentParameters();

	bool ValidateSettings();

	/**
	 * Checks for io error, validates settings and computes dependent parameters after a previous file io operation.
	 * \param ioResult  Result of previous load/save/compare file io 
	 * \param funcName  Name of the previous file io operation (for debug output)
	 * \param fileName  Filename of previous file io operation (for debug output)
	 * \return True if there was no file io error and settings were valid.
	 */
	bool FinalizeFileIO(bool ioResult, char *funcName, char *fileName);

public:
	/// \name Constructor
	/// @{ 
	Settings();
	///@} End of group started by \name

	/// \name Misc
	/// @{ 
	bool SaveBinaryFile(char *filename);
	bool LoadBinaryFile(char *filename);
	bool CompareBinaryFile(char *filename);

	bool SaveAsciiFile(char *filename);
	bool LoadAsciiFile(char *filename);
	bool CompareAsciiFile(char *filename);
	///@} End of group started by \name

	
	/// \name Setters
	/// @{ 
	void SetN(unsigned int n)			{ _n = n; }                 ///< Set number of years for backward computation
	void SetNMin(unsigned int nmin)     { _n_min = nmin; }
    void SetNFW(unsigned int n)		    { _n_fw = n; }              ///< Set number of years for forward computation
	void SetNMinFW(unsigned int nminfw) { _n_min_fw = nminfw; }
    void SetTCnt(unsigned int n)		{ _t_cnt= n; }

	void SetGridX(unsigned int x)		{ _grid_x = x; ComputeDependentParameters(); }
	void SetGridY(unsigned int y)		{ _grid_y = y; ComputeDependentParameters(); }
	void SetXMin(double v)				{ _x_min = v; ComputeDependentParameters(); }
	void SetXMax(double v)				{ _x_max = v; ComputeDependentParameters(); }
	void SetYMin(double v)				{ _y_min = v; ComputeDependentParameters(); }
	void SetYMax(double v)				{ _y_max = v; ComputeDependentParameters(); }
	void SetFitIndep(double f)			{ _fit_indep = f; ComputeDependentParameters(); }

    void SetEMax(unsigned int v)		{ _e_max = v; _e_cnt = _e_max+1; }
    void SetAMax(unsigned int v)		{ _a_max = v; ComputeDependentParameters(); }


    void SetTheta(double t)				{ _theta = t; }
    void SetPexp(double p)				{ _p_exp = p; }
	void SetNBrood(unsigned int n)		{ _n_brood = n; }
	void SetGammaIncub(double gi)		{ _gamma_incub = gi; }
	void SetGammaNest(double gn)		{ _gamma_nest = gn; }
	void SetDeltaResStart(double drs)	{ _delta_res_start = drs; }
	void SetDeltaCondStart(double dcs)	{ _delta_cond_start = dcs; }

	void SetCBmr(double cbmr)           { _c_bmr = cbmr; }

	void SetBgDisease(double bgdis)		{ _bg_disease = bgdis; }	

    void SetMigrDur(unsigned int migrDur)     { _migr_dur = migrDur; ComputeDependentParameters(); }
	void SetMMigr(double mMigr)               { _m_migr = mMigr; }
	void SetDResMigrAct(double dResMigrAct)   { _dres_migr_act = dResMigrAct; }
	void SetDResMigrPas(double dResMigrPas)   { _dres_migr_pas = dResMigrPas; }
	void SetDCondMigrAct(double dCondMigrAct) { _dcond_migr_act = dCondMigrAct; }
	void SetDCondMigrPas(double dCondMigrPas) { _dcond_migr_pas = dCondMigrPas; }

	void SetPActiveFlightConst(double pActiveFlightConst) { _p_active_flight_const = pActiveFlightConst; }
	void SetPActiveFlight(NArray<double> pActiveFlight) { _p_active_flight = pActiveFlight;}
	void SetEnvFoodSupply(NArray<double> envFoodSupply) { _env_food_supply = envFoodSupply;}

	void SetEnvironment(NArray<double> &eps,NArray<double> &abar) {
		_eps   = eps; 
		_a_bar = abar;
		_o_cnt = _eps.GetDim(0);
	}

	void SetCrit(double c)				  { _crit = c; }
	void SetStochAddReserves(bool v)	  { _stoch_add_reserves = v; }
	void SetStochAddHealth(bool v)		  { _stoch_add_health = v; }
	void SetStochFacX(double x)			  { _stochfac_x = x; }
    
	void SetIncubation(unsigned int inc)	{ _incubation = inc; ComputeDependentParameters(); }

	void SetCFuncBmr(double bmr)	{ _c_bmr = bmr; }

	///@} End of group started by \name

	// ==============================================================

	/// \name Getters
	/// @{ 
	bool   GetRunForward()		      { return _run_forward;  }
	bool   GetRunBackward()		      { return _run_backward; }
	bool   GetEnableMigration()       { return _enable_migration; }
	bool   GetEnableHealthDim()       { return _enable_health_dim; }

	unsigned int GetN()				  { return _n; }
	unsigned int GetNMin()			  { return _n_min; }
	char  *GetFilePrefix()		      { return _file_prefix; }
	bool   GetCalibrateTheta()        { return _calibrate_theta; }
	double GetCalibrationThetaMin()   { return _calibrate_theta_min; }


    unsigned int GetNFW()			  { return _n_fw; }
	unsigned int GetNMinFW()		  { return _n_min_fw; }	
	char        *GetFilePrefixFW()	  { return _file_prefix_fw; }
    unsigned int GetStartWeekFW()     { return _start_week_fw; }
	unsigned int GetStartLocationFW() { return _start_loc_fw; }

    unsigned int GetTCnt()		{ return _t_cnt; }
	unsigned int GetGridX()		{ return _grid_x; }
	unsigned int GetGridY()		{ return _grid_y; }

	double	GetXMin()			{ return _x_min; }
	double	GetXMax()			{ return _x_max; }
	double	GetYMin()			{ return _y_min; }
	double	GetYMax()			{ return _y_max; }
    double	GetDx()             { return _dx; }
    double	GetDy()             { return _dy; }
    unsigned int GetXCnt()		{ return _x_cnt; }
    unsigned int GetYCnt()		{ return _y_cnt; }


    double	GetXindep()         { return _x_indep; }
    double	GetYindep()         { return _y_indep; }
    
    unsigned int GetEMax()		{ return _e_max; }
    unsigned int GetAMax()		{ return _a_max; }

	unsigned int GetECnt()		{ return _e_cnt; }
    unsigned int GetACnt()		{ return _a_cnt; }
	unsigned int GetOCnt()      { return _o_cnt; }
	unsigned int GetSCnt()      { return _s_cnt; }
	
    double	GetTheta()			{ return _theta; }
    double	GetPexp()			{ return _p_exp; }
	unsigned int GetNBrood()	{ return _n_brood; }
	double  GetGammaIncub()		{ return _gamma_incub; }
	double  GetGammaNest()		{ return _gamma_nest; }
	double	GetFitIndep()		{ return _fit_indep; }
	double	GetDeltaResStart()	{ return _delta_res_start; }
	double	GetDeltaCondStart()	{ return _delta_cond_start; }

	double  GetCBmr()           { return _c_bmr; }

	double GetBgDisease()       {return _bg_disease;} 

	unsigned int GetMigrDur()   { return _migr_dur; }
	double  GetMMigr()          { return _m_migr; }
	double  GetdResMigrAct()    { return _dres_migr_act; }
	double  GetdResMigrPas()    { return _dres_migr_pas; }
	double  GetdCondMigrAct()   { return _dcond_migr_act; }
	double  GetdCondMigrPas()   { return _dcond_migr_pas; }

	NArray<double> GetEps()   { return _eps; }
	NArray<double> GetABar()  { return _a_bar; }
	
	double	GetCrit()			{ return _crit; }
	bool    GetStochAddReserves()  { return _stoch_add_reserves; }
	bool    GetStochAddHealth()  { return _stoch_add_health; }
	double	GetStochFacX()		{ return _stochfac_x; }
    
	unsigned int GetIncubation() { return _incubation; }

	double GetPActiveFlightConst() { return _p_active_flight_const; }
	NArray<double> & GetPActiveFlight() { return _p_active_flight; }
	NArray<double> & GetEnvFoodSupply() { return _env_food_supply; }
	///@} End of group started by \name

	/// \name Getters for FuncType references
	/// @{ 
	FuncType & CFuncBmrX()		{ return _c_bmr_x_func; }
	FuncType & CFuncU()			{ return _c_u_func; }
	FuncType & CFuncX()			{ return _c_x_func; }
	FuncType & MFuncU(int o)	{ return _m_u_func[o]; }
	FuncType & MFuncX(int o)	{ return _m_x_func[o]; }
	FuncType & MMigFuncX()      { return _m_migr_x_func; }
	FuncType & AlphaFunc()		{ return _alpha_func; }
	FuncType & DFunc()			{ return _d_func; }
	FuncType & MigActFuncX()    { return _migr_act_x_func; } 
	FuncType & MigPasFuncX()    { return _migr_pas_x_func; } 	
	///@} End of group started by \name
};

#endif // SETTINGS_H
