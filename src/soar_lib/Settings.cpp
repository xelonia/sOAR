/**
* \file Settings.cpp
* \brief Implementation of class Settings
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

#include <stdio.h>
#include <string.h>

#pragma warning ( disable: 4996) // warning C4996: 'fopen': This function or variable may be unsafe.

Settings::Settings() {           

	// ======== General settings ======== 
	_pm.Add(_run_backward,         "RunBackward");
	_pm.Add(_run_forward,          "RunForward");
	_pm.Add(_user_init_start_pop,  "UserdefinedInitializationOfStartPopulation", true);
	_pm.Add(_save_mortality_pattern_each_cycle, "SaveMortalityPatternEachCycle", true);
	_pm.Add(_save_final_mortality_pattern,      "SaveFinalMortalityPattern", true);
	_pm.Add(_enable_migration,     "EnableMigrationOption");
	_pm.Add(_enable_health_dim,    "EnableHealthDimension");

	//-- Backward general settings
	_pm.Add(_n,                    "BackwardMaximumNumberOfIterations");
	_pm.Add(_file_prefix,          "BackwardFilePrefix");
	_pm.Add(_calibrate_theta,      "BackwardCalibrateTheta");      
	_pm.Add(_calibrate_theta_min,  "BackwardCalibrateThetaMin", true);
	
	//-- Forward general settings
	_pm.Add(_n_fw,                 "ForwardMaximumNumberOfIterations");
	_pm.Add(_n_min_fw,             "ForwardMinimumNumberOfIterations", true);
	_pm.Add(_file_prefix_fw,       "ForwardFilePrefix");
	_pm.Add(_start_week_fw,        "ForwardStartEpoch");      
	_pm.Add(_start_loc_fw,         "ForwardStartLocation");

	//-- Grid for time and state variables
	_pm.Add(_t_cnt,      "DecisionEpochsPerPeriod");
	_pm.Add(_x_min,      "ReservesMin");
	_pm.Add(_x_max,      "ReservesMax");
	_pm.Add(_grid_x,     "RevervesSubdivisions");
	_pm.Add(_y_min,      "HealthLevelMin",     true); 
	_pm.Add(_y_max,      "HealthLevelMax",     true); 
	_pm.Add(_grid_y,     "HealthSubdivisions", true); 
	_pm.Add(_e_max,      "ExperienceMax");
	_pm.Add(_a_max,      "AgeOfIndependence");
	_pm.Add(_o_cnt,      "NumberOfLocations");

	//-- Numerical parameters
	_pm.Add(_crit,               "ConvergenceCriterion");
	_pm.Add(_stoch_add_reserves, "AdditionalStochasticityReserves", true);
	_pm.Add(_stoch_add_health,   "AdditionalStochasticityHealth", true);
	_pm.Add(_stochfac_x,         "StochasticityFactor",true);

	// ======== Environmental settings ======== 
	//_pm.Add(_o_cnt,           "NumberOfLocations");

	// Alternatives: Either both (AverageFoodSupply & FoodSeasonality)  or   FoodSupplyCSV
	_pm.Add(_a_bar,           "AverageFoodSupply",true);
	_pm.Add(_eps,             "FoodSeasonality",  true);
	_pm.Add(_env_food_supply, "FoodSupplyCSV",    true); 

	// Alternatives: Either ProbabilityOfActiveFlight or ProbabilityOfActiveFlightCSV
	_pm.Add(_p_active_flight_const, "ProbabilityOfActiveFlight",    true);	
	_pm.Add(_p_active_flight,       "ProbabilityOfActiveFlightCSV", true);	


	// ======== Cost and contraint ======== 
	_pm.Add(_p_exp,            "ProbabilityOfExperienceGrowth");
	_pm.Add(_theta,            "Theta", true);


	//-- Reproduction costs and constraints
	_pm.Add(_delta_res_start,  "ReserveCostsOfStartReproduction");
	_pm.Add(_gamma_incub,      "ReserveCostsOfIncubation");
	_pm.Add(_gamma_nest,       "ReserveCostsOfCareForYoung");
	_pm.Add(_delta_cond_start, "HealthCostsOfStartReproduction", true);
	_pm.Add(_incubation,       "DurationOfIncubation");
	_pm.Add(_n_brood,          "NumberOfOffspring");
	_pm.Add(_fit_indep,        "FitnessAtIndependence");


	//-- Migration costs and constraints 
	_pm.Add(_migr_dur,         "DurationOfMigration" ,                       true);
	_pm.Add(_m_migr,           "PredationRiskDuringMigration",               true);
	_pm.Add(_m_migr_x_func,    "ReserveDependencyOfPredationDuringMigration",true);
	_pm.Add(_dres_migr_act,    "ReserveCostsOfActiveFlight",  true);	
	_pm.Add(_dres_migr_pas,    "ReserveCostsOfPassiveFlight", true); 
	_pm.Add(_dcond_migr_act,   "HealthCostsOfActiveFlight",   true); 
	_pm.Add(_dcond_migr_pas,   "HealthCostsOfPassiveFlight",  true); 

	// ======== Response functions ========
	
	//-- Metabolism
	_pm.Add(_c_bmr,            "BasalMetabolicRate");
	_pm.Add(_c_bmr_x_func,     "ReserveDependencyOfBMR");
	_pm.Add(_c_u_func,         "ActivityDependencyOfMetabolism");
	_pm.Add(_c_x_func,         "ReserveDependencyOfMetabolism");

	//-- Predation
	_pm.Add(_m_x_func[0],      "ReserveDependencyOfPredationLocation1");
	_pm.Add(_m_x_func[1],      "ReserveDependencyOfPredationLocation2", true); 
	_pm.Add(_m_u_func[0],      "ActivityDependencyOfPredationLocation1");
	_pm.Add(_m_u_func[1],      "ActivityDependencyOfPredationLocation2", true); 

	//-- Flight energetics
	_pm.Add(_migr_act_x_func,  "ReserveDependencyOfActiveFlight",  true); 
	_pm.Add(_migr_pas_x_func,  "ReserveDependencyOfPassiveFlight", true); 

	//-- Health response
	_pm.Add(_bg_disease,       "BackgroundMortalityByDisease", true);	
	_pm.Add(_alpha_func,       "MetabolismDependencyOfHealth", true);	


	// Minimum number of periods in backward and forward computation
	// (No longer available in settings file)
	_n_min    = 3;
	_n_min_fw = 3;

	// All basic-type optional parameters need to be set to an invalid value 
	// to allow validity check of the configuration file. 
	// The special-type optional parameters allow other methods of validation:
	// - FuncTypes are automatically instantiated to FT_NONE 
	// - NArrays are empty.
	_user_init_start_pop   = false;
	_save_mortality_pattern_each_cycle = false;
	_save_final_mortality_pattern      = false;
	_y_min                 = -1;
	_y_max                 = -1;
	_grid_y                = 0;

	_stoch_add_reserves    = true;
	_stoch_add_health      = false;
	_stochfac_x            = -1;

	_p_active_flight_const = -1;
	_migr_dur              = 0; 
	_m_migr                = -1;

	_delta_cond_start      = -1;
	_bg_disease            = -1;

	_theta                 = -1;
	_calibrate_theta_min   = -1;
}

bool Settings::ValidateSettings()
{	
	bool okay = true;
	bool warn = false;

	printf("\n");

	// Prepare setup for optional parameters
	if ( !_enable_migration ) {
		_migr_dur = 1;
		_m_migr   = 1;
		_dres_migr_act.SetConstant(_x_max);
		_dres_migr_pas.SetConstant(_x_max);
		_dcond_migr_act.SetConstant(_y_max);
		_dcond_migr_pas.SetConstant(_y_max);
		_m_migr_x_func.SetConstant(0);

		const int checks = 12;
		char *checkList[checks] = {
			"ProbabilityOfActiveFlight",
			"DurationOfMigration",
			"PredationRiskDuringMigration",
			"ReserveDependencyOfPredationDuringMigration",
			"ReserveCostsOfActiveFlight",
			"ReserveCostsOfPassiveFlight",
			"HealthCostsOfActiveFlight",
			"HealthCostsOfPassiveFlight",
			"ReserveDependencyOfPredationLocation2",
			"ActivityDependencyOfPredationLocation2",
			"ReserveDependencyOfActiveFlight",
			"ReserveDependencyOfPassiveFlight"
		};

		// Allocate and clear outputString
		char outputString[1024];
		outputString[0]=0;

		// For each 'unused' parameter still loaded from config
		// append '- nameOfParameter\n" to outputString
		for(int c=0; c<checks;c++)
		{
			if (_pm.WasLoadedFromFile(checkList[c]) )
			{
				strcat_s(outputString,"  - ");
				strcat_s(outputString,checkList[c]);
				strcat_s(outputString,"\n");
			}
		}

		// If something was added to outputString
		if (outputString[0])
		{
			printf("Note: Migration option not enabled! \n"); 
			printf("  The following migration parameters are specified in config,\n");
			printf("  but will not come into effect: \n"); 
			printf("%s\n",outputString);
			warn = true;
		}
	}
	else
	{
		if (_migr_dur==0)  
		{
			printf("Error:  DurationOfMigration needs to be set and >=0 when Migration option is enabled!\n");  
			okay = false;
		}
	}

	if ( !_enable_health_dim ) {
		 _y_min  = _x_max;
		 _y_max  = _x_max;
		 _grid_y = 0;     
		 _bg_disease = 0; 

		const int checks = 6;
		char *checkList[checks] = {
			"HealthLevelMin",	
			"HealthLevelMax",
			"HealthSubdivisions", 	
			"BackgroundMortalityByDisease",
			"HealthCostsOfStartReproduction",
			"MetabolismDependencyOfHealth"
		};
		char *info[checks] = {
			" set equal to ReservesMax",
			" set equal to ReservesMax",
			" set equal to zero",
			" set equal to zero",
			" ineffective",
			" ineffective",
		};

		// Allocate and clear outputString
		char outputString[1024];
		outputString[0]=0;

		// For each 'unused' parameter still loaded from config
		// append '- nameOfParameter info\n" to outputString
		for(int c=0; c<checks;c++)
		{
			if (_pm.WasLoadedFromFile(checkList[c]) )
			{
				strcat_s(outputString,"  - ");
				strcat_s(outputString,checkList[c]);
				strcat_s(outputString,info[c]);
				strcat_s(outputString,"\n");
			}
		}

		// If something was added to outputString
		if (outputString[0])
		{
			printf("Note: Health dimension not enabled! \n"); 
			printf("  The following migration parameters are specified in config,\n");
			printf("  but will not come into effect: \n"); 
			printf("%s",outputString);
			printf("- Appropriate background mortality can be set via predation function.\n\n");
			warn = true;
		}
	}


	// Check validity of parameters
	if (_grid_x==0) {
		printf("Error:  Reserves subdivisions is zero!\n");  
		okay = false;
	}
	if ( _x_max <= _x_min + 0.000001) {
		printf("Error:  ReservesMax (%f) needs to be larger than ReservesMin (%f) !\n", _x_max, _x_min);  
		okay = false;
	}

	if (!_stoch_add_reserves && !_stoch_add_health)
	{
		if (_stochfac_x>=0)
		{
			printf("Note: StochasticityFactor specified in config, but ineffective since\n");
			printf("  AdditionalStochasticityReserves and AdditionalStochasticityHealth\n");
			printf("  are both FALSE\n");  
			warn = true;
		}
	}
	else if (_stochfac_x<0)
	{
		printf("Error:  StochasticityFactor needs to be set when using AdditionalStochasticityReserves or AdditionalStochasticityHealth !\n");
		okay = false;
	}


	if (_enable_health_dim)
	{
		if (_grid_y==0) {
			printf("Error:  Health subdivisions is zero though EnableHealthDimension is set!\n");  
			okay = false;
		}
		if (_y_min < 0) {
			printf("Error:  HealthLevelMin needs to be set and >=0 when health dimension is enabled!  \n");
			okay = false;
		}
		if (_y_max < 0) {
			printf("Error:  HealthLevelMax needs to be set and >=0 when health dimension is enabled!  \n");
			okay = false;
		}
		if ( _y_max <= _y_min + 0.000001) {
			printf("Error:  HealthMax (%f) needs to be larger than HealthMin (%f)!\n", _y_max, _y_min);  
			okay = false;
		}
		if (_delta_cond_start < 0) {
			printf("Error:  HealthCostsOfStartReproduction needs to be set and >=0 when health dimension is enabled!  \n");
			okay = false;
		}
		if (_bg_disease < 0) {
			printf("Error:  BackgroundMortalityByDisease needs to be set and >=0 when health dimension is enabled!  \n");
			okay = false;
		}
		if (_alpha_func.GetType() == FuncType::FuncTypeVariant::FT_NONE) {
			printf("Error:  MetabolismDependencyOfHealth needs to be set when health dimension is enabled!  \n");
			okay = false;
		}
	}

	if (_enable_health_dim && _enable_migration) {
		if (_dcond_migr_act.GetType() == FuncType::FuncTypeVariant::FT_NONE) {
			printf("Error:  HealthCostsOfActiveFlight needs to be set when migration option is enabled! \n");
			okay = false;
		}
		if (_dcond_migr_pas.GetType() == FuncType::FuncTypeVariant::FT_NONE) {
			printf("Error:  HealthCostsOfPassiveFlight needs to be set when migration option is enabled! \n");
			okay = false;
		}
	}

	if (!_enable_migration) {
		if (_o_cnt != 1) {
			printf("Error: NumberOfLocations=%u though migration option not enabled! \n", _o_cnt);
			okay = false;
		}
	}

	if (_enable_migration) {
		if (_o_cnt < 2 || _o_cnt == 0) {
			printf("Error:  NumberOfLocations=%u though migration option enabled! \n", _o_cnt);
			okay = false;
		}
		if (_migr_dur < 1) {
			printf("Error:  DurationOfMigration (%u) smaller than one decision epoch!\n", _migr_dur);  
			okay = false;
		}
		if (_migr_dur > _t_cnt) {
			printf("Warning:  DurationOfMigration (%u) larger than number of decision epochs (%u) per time cycle!\n", _migr_dur,_o_cnt);  
		}
		if (_m_migr > 1 || _m_migr < 0) {
			printf("Error:  PredationRiskDuringMigration (%f) not within range [0,1]! \n", _m_migr);  
			okay = false;
		}
		if (_m_migr < 0) {
			printf("Error: PredationRiskDuringMigration (%f) not set larger or equal to zero! \n", _m_migr);
			okay = false;
		}
		if (abs(_m_migr-1) <= 0.0000001) {
			printf("Warning:  Due to PredationRiskDuringMigration ~= (%f), migration option will never be selected.\n", _m_migr);  
		}		
		if (_dres_migr_act.GetType() == FuncType::FuncTypeVariant::FT_NONE) {
			printf("Error:  ReserveCostsOfActiveFlight needs to be set when migration option is enabled! \n");
			okay = false;
		}
		if (_dres_migr_pas.GetType() == FuncType::FuncTypeVariant::FT_NONE) {
			printf("Error:  ReserveCostsOfPassiveFlight needs to be set when migration option is enabled! \n");
			okay = false;
		}
		if (_m_migr_x_func.GetType() == FuncType::FuncTypeVariant::FT_NONE) { 
			printf("Error: ReserveDependencyOfPredationDuringMigration not set though migration option enabled! \n"); 
			okay = false;
		}
		if (_m_x_func[1].GetType() == FuncType::FuncTypeVariant::FT_NONE) { 
			printf("Error: ReserveDependencyOfPredationLocation2 not set though migration option enabled! \n"); 
			okay = false;
		}
		if (_m_u_func[1].GetType() == FuncType::FuncTypeVariant::FT_NONE) { 
			printf("Error: ActivityDependencyOfPredationLocation2 not set though migration option enabled! \n"); 
			okay = false;
		}
		// Check optional parameters
		if (_p_active_flight.GetSize() != 0) {
			if (_p_active_flight.GetDims()!=2) {
				printf("Error:  p_active_flight  dimensionality != 2\n");  
				okay = false;
		}
		else if (_p_active_flight.GetDim(0)!=_t_cnt || _p_active_flight.GetDim(1)!=_migr_dur+1) {
			printf("Error:  p_active_flight is %d x %d instead of   %d (timesteps) x %d (migration dur+1)\n",_p_active_flight.GetDim(0),_p_active_flight.GetDim(1),_t_cnt,_migr_dur+1);  
			okay = false;
		}
		}
		else {
			if (_p_active_flight_const < 0.0 || _p_active_flight_const>1.0) {
			printf("Error:  p_active_flight_const=%f  not set in range 0..1\n",_p_active_flight_const);  
			okay = false;
			}
		}
	} // end: if (_enable_migration)

	if (_calibrate_theta == false && _theta < 0) {
		printf("Error:  Set Theta >= 0 if BackwardCalibrateTheta = false. \n");  
		okay = false;
	}
	if (_calibrate_theta == true && _calibrate_theta_min < 0) {
		printf("Error:  Set BackwardCalibrateThetaMin >= 0 if BackwardCalibrateTheta = true. \n");  
		okay = false;
	}

	if (_env_food_supply.GetSize() != 0) {
		if (_env_food_supply.GetDims()!=2) {
			printf("Error:  env_food_supply  dimensionality != 2\n");  
			okay = false;
		}
		else if (_env_food_supply.GetDim(0)!=_t_cnt || _env_food_supply.GetDim(1)!=_o_cnt) {
			printf("Error:  env_food_supply is %d x %d instead of   %d (timesteps) x %d (location count)\n",_env_food_supply.GetDim(0),_env_food_supply.GetDim(1),_t_cnt,_o_cnt);  	
			okay = false;
		}
	}
	else {
		if (_eps.GetDims()!= 1 || _a_bar.GetDims()!=1) {
			printf("Error:  FoodSeasonality[%d], AverageFoodSupply[%d] need to be one dimensional\n",_eps.GetDims(),_a_bar.GetDims() );  
			okay = false;
		}

		if (_o_cnt!=_a_bar.GetDim(0) ) {
			printf("Error:  FoodSeasonality[%d], AverageFoodSupply[%d] need to be the same size as NumberOfLocations(%d)\n",_eps.GetDim(0),_a_bar.GetDim(0),_o_cnt );  
			okay = false;
		}
	}

	if (!okay || warn)
		printf("\n");

	return okay;
}

void Settings::ComputeDependentParameters()
{
	_dx = 0.000000001;
	if (_grid_x > 0) {
		_dx = (double)(_x_max-_x_min) / (double)_grid_x;
	}

	_dy = 0.000000001;
	if (_grid_y>0) {
		_dy = (double)(_y_max-_y_min) / (double)_grid_y;
	}

	_x_cnt = _grid_x + 1;
	_y_cnt = _grid_y + 1;
		
	_x_indep = _x_min + _fit_indep*(_x_max-_x_min);
	_y_indep = _y_min + _fit_indep*(_y_max-_y_min);

	_a_cnt = _a_max + _incubation+2;
	_e_cnt = _e_max + 1;
	_s_cnt = 1;
	if (_s_cnt < _migr_dur) {
		_s_cnt = _migr_dur;
	}
	
	_d_func.SetDy( _bg_disease, 1.0/ _y_max); 
}

bool Settings::FinalizeFileIO(bool ioResult, char *funcName, char *fileName)
{
	if (ioResult) {		
		printf("Validating '%s'\n",fileName);
		ioResult = ValidateSettings();
		if (ioResult) {
			ComputeDependentParameters();
			printf("%s '%s' okay\n",funcName, fileName);
		}
	}
	else {
		printf("%s '%s' error\n",funcName, fileName);
		printf("  %s\n",_pm.GetNameOfFailedParam() );
	}
	return ioResult;
}

bool Settings::SaveAsciiFile(char *filename)
{
	bool ret = _pm.SaveAscii(filename);
	return FinalizeFileIO(ret, "SaveAsciiFile",filename);
}

bool Settings::LoadAsciiFile(char *filename)
{
	bool ret = _pm.LoadAscii(filename);
	return FinalizeFileIO(ret, "LoadAsciiFile",filename);
}

bool Settings::CompareAsciiFile(char *filename)
{
	bool ret = _pm.CompareAscii(filename);
	return FinalizeFileIO(ret, "CompareAsciiFile",filename);
}

bool Settings::SaveBinaryFile(char *filename)
{
	bool ret = _pm.SaveBinary(filename);
	return FinalizeFileIO(ret, "SaveBinaryFile",filename);
}

bool Settings::LoadBinaryFile(char *filename)
{
	bool ret = _pm.LoadBinary(filename);
	return FinalizeFileIO(ret, "LoadBinaryFile",filename);
}

bool Settings::CompareBinaryFile(char *filename)
{
	bool ret = _pm.CompareBinary(filename);
	return FinalizeFileIO(ret, "CompareBinaryFile",filename);
}
