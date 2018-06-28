/**
* \file UtBackward.cpp
* \brief Implementation of UtBackward to test the Backward class
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#include "UnitTest.h"

#include "../soar_lib/Decision.h"
#include "../soar_lib/Backward.h"
#include "../soar_lib/Settings.h"


/**
 * \ingroup Tests
 * \brief UnitTest for Backward class
 */
class UtBackward : public UnitTest {
private:

public:

	/// \brief The constructor registers at the UnitTestManager 
	UtBackward() : UnitTest("Backward")
	{
	}
	
	/**
	 * \ingroup Tests
	 * \brief Manages statistics of Backward runs
	 */
	class UtBackwardStats {
	private:
		ParamManager   _pm;			///< Manager for load/save of state variables

		NArray<double> _lambda;		///< Lambda of each year
		NArray<int>    _stateC;		///< Number of decision elements in Care state for each year and tick
		NArray<int>    _stateN;		///< Number of decision elements in NoCare state for each year and tick
		NArray<int>    _stateM;		///< Number of decision elements in Migrate state for each year and tick
		NArray<int>    _stateS;		///< Number of decision elements in Start state for each year and tick

	public:
		/// \brief Constructor
		UtBackwardStats() {
			_pm.Add(_lambda,"Lambda");
			_pm.Add(_stateC,"StateC");
			_pm.Add(_stateN,"StateN");
			_pm.Add(_stateM,"StateM");
			_pm.Add(_stateS,"StateS");
		}

		/// \brief Initialize arrays
		void Init(Settings &settings) 
		{		
			int years = settings.GetN();
			int ticks = settings.GetTCnt();
			int locs  = settings.GetOCnt();

			_lambda.Init(years );
			_stateC.Init(locs,years,ticks);
			_stateN.Init(locs,years,ticks);
			_stateM.Init(locs,years,ticks);
			_stateS.Init(locs,years,ticks);
		}


		/// \brief Update statistics for all ticks of a given year
		void Update(Settings &settings, Decision &decision) 
		{
			int year      = decision.GetYear() -1;
			_lambda(year) = decision.GetLambda();
			
			int xCnt = settings.GetXCnt();
			int yCnt = settings.GetYCnt();
			int eCnt = settings.GetECnt();
			int aCnt = settings.GetACnt();

			int oCnt = settings.GetOCnt();
			int sCnt = settings.GetSCnt();
			int ticks = settings.GetTCnt();
			for(int tick=0; tick<ticks; tick++) {
				for(int o=0; o<oCnt; o++) {
					int cState=0,mState=0,nState=0,sState=0;

					for(int x=0; x<xCnt; x++) {
						for(int y=0; y<yCnt; y++) {
							for(int e=0; e<eCnt; e++) {
								for(int a=0; a<aCnt; a++) {
									for(int s=0; s<sCnt; s++) {

										char state = decision.GetF_strat(x,y,e,a,o,s,tick);
										switch(state) 
										{
										case 'c' : cState++; break;
										case 's' : sState++; break;
										case 'm' : mState++; break;
										case 'n' : nState++; break;
										default: break;
										}
									}
								}
							}
						}
					}

					_stateC(o,year,tick) = cState;
					_stateS(o,year,tick) = sState;
					_stateM(o,year,tick) = mState;
					_stateN(o,year,tick) = nState;
				}

			}
		}

		/// \brief Check if lambda converges over the years
		bool CheckLamdbaTrend() {
			// Lambda's delta to 1 should shrink 
			if (_lambda.GetDim(0)<3)
			{
				printf("CheckLamdbaTrend : Not enough years (%d)\n",_lambda.GetDim(0));
				return true;
			}
			// Ignore year zero
			double deltaBest = fabs(_lambda(1) - 1.0);
			double deltaAllowed = 0.01;
			for(unsigned int y = 2; y<_lambda.GetDim(0); y++) {
				double delta = fabs(_lambda(y) - 1.0);
				if (delta> deltaBest +deltaAllowed) // Allow variation in nth decimal...
				{
					printf("CheckLamdbaTrend: y=%d  curr=%f  best=%f\n",y,delta,deltaBest);
					return false;
				}

				if (y>=7)
					deltaAllowed = 0.0001;
				else if (y>=4)
					deltaAllowed = 0.001;

				deltaBest = delta;
			}
			return true;
		}

		/// \brief Check that the other result has the same number of years and ticks
		bool IsSameLayout(UtBackwardStats &other) {
			bool same = true;


			// Check number of years in lambda
			if (_lambda.GetDim(0) != other._lambda.GetDim(0) ) {
				printf("Number of lambda years differ in results file (%d vs %d)\n",_lambda.GetDim(0), other._lambda.GetDim(0));

				// Ignore if current calculation has less years than result
				if (_lambda.GetDim(0) > other._lambda.GetDim(0))
					same = false;
			}

			// Check Dimensions
			if (_stateC.GetDims() != other._stateC.GetDims() ) {
				printf("Dimensions of stateC differ in results file (%d vs %d)\n",_stateC.GetDims(), other._stateC.GetDims());
				return false;
			}

			// Check number of locations
			if (_stateC.GetDim(0) != other._stateC.GetDim(0) ) {
				printf("Number of locations differ in results file (%d vs %d)\n",_stateC.GetDim(0),other._stateC.GetDim(0));
				same = false;
			}

			// Check number of years
			if (_stateC.GetDim(1) != other._stateC.GetDim(1) ) {
				printf("Number of years differ in results file (%d vs %d)\n",_stateC.GetDim(1),other._stateC.GetDim(1));

				// Ignore if current calculation has less years than result
				if (_stateC.GetDim(1) > other._stateC.GetDim(1))
					same = false;
			}

			// Check ticks
			if (_stateC.GetDim(2) != other._stateC.GetDim(2) ) {
				printf("Number of ticks differ in results file  (%d vs %d)\n",_stateC.GetDim(2), other._stateC.GetDim(2));
				same = false;
			}

			return same;
		}


		/// \brief Check all result of a specific year versus the other results
		bool IsYearSameAs(UtBackwardStats &other, int y) {
			if ( _lambda(y) >1000 && other._lambda(y)>1000) {
				if ( fabs( _lambda(y) - other._lambda(y) ) >0.0001 * _lambda(y) ) {
					printf("Lambda(%d) differs in result file (big value) %f %f  delta=%f\n",y,_lambda(y),other._lambda(y),fabs( _lambda(y) - other._lambda(y) ));
					return false;
				}
			}
			else if ( fabs( _lambda(y) - other._lambda(y) ) > 0.00001) {
				printf("Lambda(%d) differs in result file\n",y);
				return false;
			}

			int differences = 0;
			for(unsigned int o=0; o<_stateC.GetDim(0); o++) {
				for(unsigned int t=0; t<_stateC.GetDim(2); t++) {
					// Hack for bad 'first values'
					if (y==0 && t==_stateC.GetDim(2)-1)
						continue;

					if ( _stateC(o,y,t) != other._stateC(o,y,t) ) {
						printf("_stateC(%d,%d)=%d differs %d from result file\n",y,t,_stateC(o,y,t),other._stateC(o,y,t));
						differences++;
					}
					if ( _stateS(o,y,t) != other._stateS(o,y,t) ) {
						printf("_stateS(%d,%d)=%d differs %d from result file\n",y,t,_stateS(o,y,t),other._stateS(o,y,t));
						differences++;
					}
					if ( _stateM(o,y,t) != other._stateM(o,y,t) ) {
						printf("_stateM(%d,%d)=%d differs %d from result file\n",y,t,_stateM(o,y,t),other._stateM(o,y,t));
						differences++;
					}
					if ( _stateN(o,y,t) != other._stateN(o,y,t) ) {
						printf("_stateN(%d,%d)=%d differs %d from result file\n",y,t,_stateN(o,y,t),other._stateN(o,y,t));
						differences++;
					}
				}
			}
			if (differences>0)
			{
				printf("Total number of state differences = %d\n",differences);
				return false;
			}
			return true;
		}

		/// \brief Check all results versus the other results
		bool IsSameAs(UtBackwardStats &other) {
			for(unsigned int y=0; y<_lambda.GetDim(0); y++) {
				if (!IsYearSameAs(other, y) )
					return false;
			}

			return true;
		}

		/// \brief Save results to ascii file
		bool Save(char *filename) {
			return _pm.SaveAscii(filename);
		}

		/// \brief Load results from ascii file
		bool Load(char *filename) {
			return _pm.LoadAscii(filename);
		}
	};


/*
	void ConvertSettings(char *name) {
		Settings set;

		char inpName[256];
		char outName[256];

		sprintf_s(inpName,"../settings/%s_new.cfg", name );
		sprintf_s(outName,"%s/Input_%s_org.cfg",GetInputPath(),name );

		if (!set.LoadAsciiFile(inpName) ) {
			fprintf(stderr,"Convert: File not found   %s\n",inpName);
			return;
		}
		set.OrgInitParamManager();
		set.OrgSaveAsciiFile(outName);
		fprintf(stderr,"Convert: %s to %s\n",inpName,outName);

	}
*/

	NArray<double> deltaF;

	void InitDeltaF(Decision &decision)
	{
		unsigned int xDim = decision.GetF().GetDim(0);
		unsigned int yDim = decision.GetF().GetDim(1);
		unsigned int eDim = decision.GetF().GetDim(2);
		unsigned int aDim = decision.GetF().GetDim(3);
		unsigned int oDim = decision.GetF().GetDim(4);
		unsigned int sDim = decision.GetF().GetDim(5);
		unsigned int tDim = decision.GetF().GetDim(6);

		unsigned int x = xDim / 2;
		unsigned int y = yDim / 2;

		deltaF.Init(eDim, aDim, oDim, sDim, tDim );

		for (unsigned int e=0;e<eDim;e++) {
			for (unsigned int a=0;a<aDim;a++)	{
				for (unsigned int o=0; o<oDim; o++) {
					for (unsigned int s=0; s<sDim; s++) {
						for (unsigned int t=0;t<tDim;t++) {
							deltaF(e,a,o,s,t) = decision.GetF(x,y,e,a,o,s,t);
						}
					}
				}
			}
		}
	}

	void UpdateAndSaveDeltaF(char *newName, Decision &decision)
	{
		unsigned int xDim = decision.GetF().GetDim(0);
		unsigned int yDim = decision.GetF().GetDim(1);
		unsigned int eDim = decision.GetF().GetDim(2);
		unsigned int aDim = decision.GetF().GetDim(3);
		unsigned int oDim = decision.GetF().GetDim(4);
		unsigned int sDim = decision.GetF().GetDim(5);
		unsigned int tDim = decision.GetF().GetDim(6);

		unsigned int x = xDim / 2;
		unsigned int y = yDim / 2;

		for (unsigned int e=0;e<eDim;e++) {
			for (unsigned int a=0;a<aDim;a++)	{
				for (unsigned int o=0; o<oDim; o++) {
					for (unsigned int s=0; s<sDim; s++) {
						for (unsigned int t=0;t<tDim;t++) {
							deltaF(e,a,o,s,t) = decision.GetF(x,y,e,a,o,s,t) - deltaF(e,a,o,s,t);
						}
					}
				}
			}
		}

		ParamManager pm;

		pm.Add(deltaF,"DeltaF");
		pm.SaveAscii(newName);
	}

	bool CompareDimsDeltaF(NArray<double> &origDeltaF)
	{
		if (deltaF.GetDims() != origDeltaF.GetDims() )
			return false;

		for(unsigned int d=0; d<deltaF.GetDims();d++) {
			if (origDeltaF.GetDim(d) != deltaF.GetDim(d) ) {
				return false;
			}
		}
		return true;
	}

	bool CompareValsDeltaF(NArray<double> &origDeltaF)
	{
		for (unsigned int e=0;e<deltaF.GetDim(0);e++) {
			for (unsigned int a=0;a<deltaF.GetDim(1);a++)	{
				for (unsigned int o=0; o<deltaF.GetDim(2); o++) {
					for (unsigned int s=0; s<deltaF.GetDim(3); s++) {
						for (unsigned int t=0;t<deltaF.GetDim(4);t++) {

							if ( fabs(deltaF(e,a,o,s,t) - origDeltaF(e,a,o,s,t) ) > 0.00001 ) {
								return false;
							}							
						}
					}
				}
			}
		}

		return true;
	}

	bool SaveF(char *name, Decision &decision)
	{
		FILE *file = 0;
		fopen_s(&file,name,"w");
		if (file==0) {
			printf("UtBackward::SaveF(%s) error opening the file for writing\n",name);
			return false;
		}
		
		unsigned int xDim = decision.GetF().GetDim(0);
		unsigned int yDim = decision.GetF().GetDim(1);
		unsigned int eDim = decision.GetF().GetDim(2);
		unsigned int aDim = decision.GetF().GetDim(3);
		unsigned int oDim = decision.GetF().GetDim(4);
		unsigned int sDim = decision.GetF().GetDim(5);
		unsigned int tDim = decision.GetF().GetDim(6);

		for (unsigned int x=0;x<xDim;x++) {
			for (unsigned int y=0;y<yDim;y++) {
				for (unsigned int e=0;e<eDim;e++) {
					for (unsigned int a=0;a<aDim;a++)	{
						for (unsigned int o=0; o<oDim; o++) {
							for (unsigned int s=0; s<sDim; s++) {

								fprintf(file,"%d;%d;%d;%d;%d;%d;",x,y,e,a,o,s);
								for (unsigned int t=0;t<tDim;t++) {
									char valStr[32];

									sprintf_s(valStr,"%f;",decision.GetF(x,y,e,a,o,s,t) );
									char *dot = strchr(valStr,'.');
									if (dot) {
										*dot = ',';
									}
									fprintf(file,"%s",valStr);
								}
								fprintf(file,"\n");
							}
						}
					}
				}
			}
		}

		fclose(file);
		return true;
	}

	/// \brief Test backward simulation for a given setting
	void TestBackwardWithSetting(char *test) {
		char groupA[128],groupB[128],groupC[128];

		sprintf_s(groupA,"%s Convergence",test);
		sprintf_s(groupB,"%s Results",test);
		sprintf_s(groupC,"%s Multi-year",test);

		//----------------- Convergence
		TestGroup(groupA);

		UtBackwardStats expectStats, resultStats;
		char setName[512], resName[512] , savName[512], ndfName[512], rdfName[512];

		sprintf_s(setName,"%s/Input_%s.cfg",      GetInputPath(),test );
		sprintf_s(resName,"%s/Input_%s_BW_Result.txt",GetInputPath(),test );
		sprintf_s(savName,"Ut_%s_BW_Result.txt",test );
		sprintf_s(rdfName,"%s/Input_%s_BW_DeltaF.txt",GetInputPath(),test );
		sprintf_s(ndfName,"Ut_%s_BW_DeltaF.txt",test );

		Settings settings;
		Decision decision;
		Backward backward;

		if (!ExpectOkay( settings.LoadAsciiFile(setName), "Loading '%s', see Logfile for details",setName) )
			return;

		backward.SetDecision(&decision);
		resultStats.Init(settings);

		int    years = settings.GetN();
		double theta = settings.GetTheta();				

		settings.SetN(1);
		settings.SetNMin(1);
		decision.SetYear(0);

		for(int y = 0; y<years; y++) {
			double lambda = backward.Compute(&settings, theta);
			resultStats.Update(settings, decision);
			
			if (y==1) {
				InitDeltaF(decision);
			}
			else if (y==2) {
				UpdateAndSaveDeltaF(ndfName, decision);
			}

#ifdef false
			// Save decision data to CSVs per year
			char fnName[512];
			sprintf_s(fnName,"Ut_%s_BW_F_%02d.csv",test,y );
			SaveF(fnName, decision);
#endif
		}

		resultStats.Save(savName);

		// Check if lambda tends to 1 over time
		ExpectOkay(resultStats.CheckLamdbaTrend(),"Lamda does not converge");

		//----------------- Results
		TestGroup(groupB);

		if (ExpectOkay( expectStats.Load(resName), "Results file '%s' not found",resName) ) {

			// Check layout
			bool ok = ExpectOkay(resultStats.IsSameLayout(expectStats),"Result file layout in '%s' differ",resName);

			if (ok) {
				// Check all statC values of results vs expected
				ExpectOkay(resultStats.IsSameAs(expectStats),"Stat results differ");
			}
		}

		/*
		// Check f development on center Reserves and Health
		{
			NArray<double> origDeltaF;

			ParamManager pm;
			pm.Add(origDeltaF,"DeltaF");

			if (ExpectOkay(pm.LoadAscii(rdfName),"CompareDeltaF load failed") ) {
				ExpectOkay( CompareDimsDeltaF(origDeltaF),"DeltaF dims differ");
				ExpectOkay( CompareValsDeltaF(origDeltaF),"DeltaF results differ");
			}
		}
		*/

		//----------------- multi-year versus the single-year runs
		TestGroup(groupC);

		Decision        decisionB;
		UtBackwardStats resultStatsB;
		backward.SetDecision(&decisionB);

		settings.SetN( (years/2)+1); // Only calc half of the years...
		settings.SetNMin(1);
		decision.SetYear(0);

		resultStatsB.Init(settings);

		double lambdaB = backward.Compute(&settings, theta);
		resultStatsB.Update(settings, decisionB);

		// Check all values of results vs expected
		ExpectOkay(resultStats.IsYearSameAs(resultStats, years-1),"Results of multi-year differ from N*single-year");

		TestGroup();
	}

	void RunTests() {
		TestBackwardWithSetting("Migration_10x10");
		TestBackwardWithSetting("Reproduction_4x4");
		TestBackwardWithSetting("Reproduction_16x16");
		TestBackwardWithSetting("AddStochNone");
		TestBackwardWithSetting("AddStochHealth");
		TestBackwardWithSetting("AddStochResHealth");
		TestBackwardWithSetting("NoHealth");
		TestBackwardWithSetting("NoMigr");
	}
};

UtBackward test_Backward;  ///< Global instance automatically registers to UnitTestManager
