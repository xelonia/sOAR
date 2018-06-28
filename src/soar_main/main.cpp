/**
* \file main.cpp
* \brief  Main class with inline implementation and executable main() entry
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

#include <stdio.h>
#include <stdlib.h>

#include "../soar_lib/Backward.h"
#include "../soar_lib/Forward.h"

#include "../soar_lib/Decision.h"
#include "../soar_lib/Settings.h"
#include "../soar_lib/Optimizer.h"

#include "../soar_support_lib/NanoTimer.h"

/**
* \defgroup Main sOAR_Main
* This module contains the Main class for the soar executable that runs backward and forward simulation
*/

/**
* \ingroup Main
* \brief Contains all the simulation parameters used in forward and backward optimization
*/
class Main {
private:
	Settings * _settings;
	Decision * _decision;

	char _setting_filename[FILENAME_MAX];
	char _filename_bw_dec[FILENAME_MAX];
	char _filename_bw_set[FILENAME_MAX];	
	char _filename_bw_sum[FILENAME_MAX];

	char _filename_fw_dec[FILENAME_MAX];
	char _filename_fw_pd[FILENAME_MAX];  // PopulationDynamics

#ifdef GENERATE_DOXYGEN_DOC
	Forward         * doxygen;  ///< DOXYGEN: Main() does a forward simulation
	Backward        * doxygen;  ///< DOXYGEN: Main() does a backward simulation
	Optimizer       * doxygen;  ///< DOXYGEN: Main() can optimize Theta with several backward simulations using the Optimizer
#endif	

	bool Init(int argc, const char * argv[]) {
		_setting_filename[0] = 0;

		_settings = 0;
		_decision = 0;

		bool showUsage = false;
		int i=1;
		while(i<argc) {
			if (strcmp(argv[i], "-config")==0 && i+1<argc) {
				strcpy_s(_setting_filename,argv[i+1]);
				i++;
			}
			else {
				showUsage = true;
			}
			i++;
		}

		if (_setting_filename[0]==0) {
			showUsage = true;
		}

		if (showUsage) {
			printf("============================\n");
			printf("Usage:\n");
			printf("  sOAR -config filename\n");
			printf("\n");
			printf("Current parameter:\n");
			for(int i=1;i<argc; i++) {
				printf("[%d]  %s\n",i,argv[i]);
			}

			exit(-1);
		}

		_settings = new Settings();

		int len = strlen(_setting_filename);
		if ( len > 4 && _stricmp( &_setting_filename[len-4], ".cfg") == 0) {
			// Try to load ascii config
			if (!_settings->LoadAsciiFile( _setting_filename ) ) {
				return false;
			}
		}
		else {
			// Try to load binary config 
			if (!_settings->LoadBinaryFile( _setting_filename ) ) {
				return false;
			}
		}

		sprintf_s(_filename_bw_dec, "%s_decision_BW.bin", _settings->GetFilePrefix() );
		sprintf_s(_filename_bw_set, "%s_settings_BW.bin", _settings->GetFilePrefix()); 
		sprintf_s(_filename_bw_sum, "%s_summary_BW.txt",  _settings->GetFilePrefix()); 

		sprintf_s(_filename_fw_dec, "%s_decision_FW.bin", _settings->GetFilePrefixFW());  
		sprintf_s(_filename_fw_pd , "%s_populationdynamics_FW.bin", _settings->GetFilePrefixFW());  

		_decision = new Decision();

		return true;
	}

	void PrintFuncs(Settings &set) {
		char c_debug[256];
		char a_debug[256];
		char d_debug[256];
		char m0_debug[256];
		char m1_debug[256];
		char mmig_debug[256];

		strcpy_s(a_debug, sizeof(a_debug), set.AlphaFunc().ToString("c") );
		strcpy_s(d_debug, sizeof(d_debug), set.DFunc().ToString("y") );

		strcpy_s(m0_debug, sizeof(m0_debug), set.MFuncU(0).ToString("u") );
		if (!set.MFuncX(0).IsZero() ) {
			strcat_s(m0_debug,sizeof(m0_debug)," * (1+ ");
			strcat_s(m0_debug,sizeof(m0_debug),set.MFuncX(0).ToString("x") );
			strcat_s(m0_debug,sizeof(m0_debug)," )");
		}

		strcpy_s(m1_debug, sizeof(m1_debug), set.MFuncU(0).ToString("u") );
		if (!set.MFuncX(1).IsZero() ) {
			strcat_s(m1_debug,sizeof(m1_debug)," * (1+ ");
			strcat_s(m1_debug,sizeof(m1_debug),set.MFuncX(1).ToString("x") );
			strcat_s(m1_debug,sizeof(m1_debug)," )");
		}

		sprintf_s(c_debug,sizeof(c_debug),"%g",set.GetCBmr() );
		if (!set.CFuncBmrX().IsZero() ) {
			strcat_s(c_debug,sizeof(c_debug)," * (1 + ");
			strcat_s(c_debug,sizeof(c_debug), set.CFuncBmrX().ToString("x") );
			strcat_s(c_debug,sizeof(c_debug)," )");
		}
		strcat_s(c_debug,sizeof(c_debug)," + ");
		strcat_s(c_debug,sizeof(c_debug), set.CFuncU().ToString("u") );
		if (!set.CFuncX().IsZero() ) {
			strcat_s(c_debug,sizeof(c_debug)," * (1+ ");
			strcat_s(c_debug,sizeof(c_debug), set.CFuncX().ToString("x") );
			strcat_s(c_debug,sizeof(c_debug)," ) ");
		}

		sprintf_s(mmig_debug,sizeof(mmig_debug),"%g",set.GetMMigr() );
		if (!set.MMigFuncX().IsZero() ) {
			strcat_s(mmig_debug,sizeof(mmig_debug)," * (1 + ");
			strcat_s(mmig_debug,sizeof(mmig_debug), set.MMigFuncX().ToString("x") );
			strcat_s(mmig_debug,sizeof(mmig_debug)," )");
		}

		printf("Alpha => %s\n",a_debug);
		printf("D     => %s\n",d_debug);
		printf("C     => %s\n",c_debug);
		printf("M0    => %s\n",m0_debug);
		printf("M1    => %s\n",m1_debug);	
		printf("MMig  => %s\n\n",mmig_debug);
	}


	void RunBackwardSimulationToOptimizeForTheta() {
		Backward bwOpt;

		bwOpt.SetDecision( _decision);

		/// \brief Wrapper structure for Brent_fmin() optimizer callback to find the best theta
		class ThetaOptimizerFunc : public Optimizer::OptimizerFunc {
		private:
			Backward *_bw;		///< Pointer to backward computation instance
			Settings *_set;		///< Pointer to settings instance
			Decision *_dec;		///< Pointer to decision instance
			int       _opIter;	///< Number of optimization iterations
			int       _bwIter;	///< Total number of Backward iterations
		public:
			ThetaOptimizerFunc(Backward *bw, Settings *set, Decision *dec) : _bw(bw), _set(set), _dec(dec), _opIter(1),_bwIter(0) {}

			double operator()(double theta) { 				
				printf("\nOptimizeTheta %d: Solving for Theta=%f \n",_opIter, theta);
				printf("-------------------------------------------\n");
				
				_dec->Reset(); // Start Backward iterations with a fresh, un-initialized decision state

				double lambda = _bw->Compute(_set, theta);
				_bwIter += _dec->GetYear();
				_opIter++;
				return fabs(lambda - 1.0);  // Optimizer tries to find minimum, but here we need to find 1.0 (ESS)
			}

			int GetOptimizerIterations()	{ return _opIter; }
			int GetBackwardIterations()		{ return _bwIter; }
		};
		
		Optimizer optimizer;
		ThetaOptimizerFunc tof(&bwOpt, _settings, _decision);
		double bestTheta = optimizer.Brent_fmin(_settings->GetCalibrationThetaMin(),1.0, tof, 1.0e-40);

		// Then the optimizer finishes, the descision will contain the results of the last 
		// run and therefore the ones for bestTheta

		double lambda       = _decision->GetLambda();
		double lambda_state = _decision->GetLambdaState();
		printf("\n\n");
		printf("============================================================\n");
		printf("    Using %d Optimizer and %d Backward iterations:\n\n",
			tof.GetOptimizerIterations(),tof.GetBackwardIterations() );
		printf("        Best theta found: %f  ()\n\n", bestTheta );
		printf("    Corresponding lambda:       %f\n",lambda);
		printf("    Corresponding lambda_state: %f\n",lambda_state);
	}

public:

	~Main() {
		delete _settings;
		delete _decision;
	}

	int Compute(int argc, const char * argv[]) {
		if ( !Init(argc, argv) )
			return -1;

		// PrintFuncs(*_settings); // NEWly outcommented.
		printf("\n");	// NEW

		if (_settings->GetRunBackward() ) {
			printf("########## Running Backward::Compute()\n" );
			NanoTimer timerTotal;
			timerTotal.Start();       

			if ( _settings->GetCalibrateTheta() ) {
				RunBackwardSimulationToOptimizeForTheta();
			}
			else {
				Backward bwOpt;

				bwOpt.SetDecision(_decision);

				double theta = _settings->GetTheta();				
				double lambda = bwOpt.Compute(_settings, theta);
				printf("\n");
				printf("      Backward lambda found: %.4f\n\n",lambda);
			}

			timerTotal.Stop();

			printf("============================================================\n");
			float time = timerTotal.GetNanoSeconds()/1000;
			printf("Backward computation time:  %10.3f mSec\n",time );			

			printf("\n ... saving backward results to file ...\n \n");
			_decision->SaveToFile(_filename_bw_dec);

			_settings->SaveBinaryFile(_filename_bw_set);

			_decision->PrintSummaryStatistics();
			printf("\n ... saving summary statistics to file ...\n");
			_decision->SaveSummaryStatisticsToFile(_filename_bw_sum);
			printf("\n... done ... \n\n");
		}
		else {
			// No backward calculation, so _decision needs to be loaded from file
			if ( !_decision->LoadFromFile(_filename_bw_dec) ) {
				exit(-1);
			}
		}


		if (_settings->GetRunForward() ) {

			//// --------- NEW (start) -----------------------------
			//if ( _settings->GetUserInitStartPop() ) {	
			//	if ( !_decision->LoadFromFile(_filename_bw_dec) ) {	 
			//	exit(-1); 
			//	}	
			//}	
			//// --------- NEW (end) -------------------------------

			printf("########## Running Forward::Compute()\n");

			NanoTimer timerTotal;
			timerTotal.Start(); 

			Forward fwOpt;

			fwOpt.SetDecision( _decision );
			double lambda;
		
			{
				lambda = fwOpt.ComputePopulationDynamics( _settings);  // cf. Toekoelyi
				if (_settings->GetUserInitStartPop()) {
					sprintf_s(_filename_fw_pd, "%s_populationdynamics_FW_finalYear.bin", _settings->GetFilePrefixFW());
					fwOpt.SavePopulationDynamics(_filename_fw_pd);
				}
				else {
					fwOpt.SavePopulationDynamics(_filename_fw_pd);
				}
			}


			printf("\n");
			printf("      Forward lambda found: %.4f\n\n",lambda);

			timerTotal.Stop();

			printf("#######################################################\n");
			float time = timerTotal.GetNanoSeconds()/1000;
			printf("Forward computation time:  %10.3f mSec\n",time );

			printf("\n ... saving forward results to file ...\n");
			_decision->SaveToFile(_filename_fw_dec); 

			printf("\n ...done... \n");
		}

		return 0;
	}
};


/// The main funcion
int main(int argc, const char * argv[]) {
	Main m;

	int ret = m.Compute(argc,argv);
	char c = getchar();
	printf("...quitting... \n");

	exit(ret);
}

