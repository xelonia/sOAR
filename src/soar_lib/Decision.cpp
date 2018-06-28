/**
* \file Decision.cpp
* \brief Implementation of class Decision
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
#include "Decision.h"

#pragma warning ( disable: 4996) // warning C4996: 'fopen': This function or variable may be unsafe.

Decision::Decision() {
	Reset();
}

Decision::~Decision() {
}

void Decision::Reset() {
	_initialized = false;
	_lambda           = 0;
	_lambda_worst     = 0;
	_bw_not_converged = 0;
	_bw_state_count   = 0;
	_year             = 0;
}

void Decision::InitDimensions(int xDim, int yDim, int eDim, int aDim, int oDim, int sDim, int tDim) {
	if (_initialized) {
		printf("Decision::initDimensions() already initialized\n");
		return;
	}

	_f.Init(xDim,yDim,eDim,aDim,oDim,sDim,tDim);
	_f_u.Init(xDim,yDim,eDim,aDim,oDim,sDim,tDim);
	_f_strat.Init(xDim,yDim,eDim,aDim,oDim,sDim,tDim);

	_initialized = true;
}

void   Decision::SetF(int x, int y, int e, int a, int o, int s, int t, double value) 
{
	_f(x,y,e,a,o,s,t) = value;
}
double Decision::GetF(int x, int y, int e, int a,  int o, int s, int t)
{
	return _f(x,y,e,a,o,s,t);
}

void   Decision::SetF_u(int x, int y, int e, int a,  int o, int s, int t, double value)
{
	_f_u(x,y,e,a,o,s,t) = value;
}
double Decision::GetF_u(int x, int y, int e, int a,  int o, int s, int t)
{
	return _f_u(x,y,e,a,o,s,t);
}

void   Decision::SetF_strat(int x, int y, int e, int a,  int o, int s, int t, char value)
{
	_f_strat(x,y,e,a,o,s,t) = value;
}
char   Decision::GetF_strat(int x, int y, int e, int a,  int o, int s, int t)
{
	return _f_strat(x,y,e,a,o,s,t);
}

void   Decision::SetF_all(int x, int y, int e, int a, int o, int s, int t, double f, double f_u, char f_strat)
{
	_f(      x,y,e,a,o,s,t) = f;
	_f_u(    x,y,e,a,o,s,t) = f_u;
	_f_strat(x,y,e,a,o,s,t) = f_strat;
}



bool   Decision::SaveToFile(char *filename) 
{
	FILE *file = fopen(filename,"wb");
	if (file==0) {
		printf("Decision::save_to_file(%s) error opening the file for writing\n",filename);
		return false;
	}

	_f.SaveBinary(file);
	_f_u.SaveBinary(file);
	_f_strat.SaveBinary(file);

	fwrite(&_lambda,1,sizeof(double), file);
	fwrite(&_theta,1,sizeof(double), file);
	fwrite(&_converged,1,sizeof(bool), file);
	fwrite(&_year,1,sizeof(int), file);
	fwrite(&_lambda_state,1,sizeof(double), file);     
	fwrite(&_lambda_worst,1,sizeof(double), file);     
	fwrite(&_bw_not_converged,1,sizeof(int), file);    
	fwrite(&_bw_state_count,1,sizeof(int), file);      
	
	fclose(file);
	return true;
}

bool Decision::LoadFromFile(char *filename) 
{
	FILE *file = fopen(filename,"rb");
	if (file==0) {
		printf("Decision::load_from_file(%s) error opening the file for reading\n",filename);
		return false;
	}

	if (!_f.LoadBinary(file) ) {
		printf("Decision::load_from_file(%s) error  file read error on 'f'\n",filename);
		return false;
	}

	if (!_f_u.LoadBinary(file) ) {
		printf("Decision::load_from_file(%s) error  file read error on 'f_u'\n",filename);
		return false;
	} 

	if (!_f_strat.LoadBinary(file) ) {
		printf("Decision::load_from_file(%s) error  file read error on 'f_strat'\n",filename);
		return false;
	} 
	
	fread(&_lambda,1,sizeof(double), file);
	fread(&_theta,1,sizeof(double), file);
	fread(&_converged,1,sizeof(bool), file);
	fread(&_year,1,sizeof(int), file);
	fread(&_lambda_state,1,sizeof(double), file);    
	fread(&_lambda_worst,1,sizeof(double), file);     
	fread(&_bw_not_converged,1,sizeof(int), file);    
	fread(&_bw_state_count,1,sizeof(int), file);      

	fclose(file);

	_initialized = true;

	return true;
}


void Decision::PrintSummaryStatistics() {
	WriteSummaryStatistics(	stdout, false );
}

void Decision::SaveSummaryStatisticsToFile(char *filename) {
	FILE *file = fopen(filename, "w");

	if (file==0) {
		printf("Decision::SaveSummaryStatistics(%s) error opening the file for writing\n",filename);
		return;
	}
	WriteSummaryStatistics(	file, true );
	fclose(file);
}


// Print summary statistics either to console or into a file


void Decision::UpdateMinMaxStat(DcStatisticsStruct &min, DcStatisticsStruct &max, int x, int y, int e, int a, int o, int s, int t)
{
	min.cnt++;
	if (min.x > x)  min.x = x;
	if (min.y > y)  min.y = y;
	if (min.e > e)  min.e = e;
	if (min.a > a)  min.a = a;
	if (min.o > o)  min.o = o;
	if (min.s > s)  min.s = s;
	if (min.t > t)  min.t = t;

	max.cnt++;
	if (max.x < x)  max.x = x;
	if (max.y < y)  max.y = y;
	if (max.e < e)  max.e = e;
	if (max.a < a)  max.a = a;
	if (max.o < o)  max.o = o;
	if (max.s < s)  max.s = s;
	if (max.t < t)  max.t = t;
}

void Decision::WriteSummaryStatistics(FILE *file, bool weeklyOutput) {

	fprintf(file, "=========== Summary Statistics ========\n");

	int xDim,yDim,eDim,aDim,oDim,sDim,tDim;

	xDim = _f.GetDim(0);
	yDim = _f.GetDim(1);
	eDim = _f.GetDim(2);
	aDim = _f.GetDim(3);
	oDim = _f.GetDim(4);
	sDim = _f.GetDim(5);
	tDim = _f.GetDim(6);

	DcStatisticsStruct sTotalMin, sTotalMax,cTotalMin, cTotalMax,nTotalMin, nTotalMax, mTotalMin, mTotalMax;

	sTotalMin.InitToValue(1000000); // maximal value
	cTotalMin.InitToValue(1000000); 
	nTotalMin.InitToValue(1000000); 
	mTotalMin.InitToValue(1000000); 

	sTotalMax.InitToValue(      0); // minimal value
	cTotalMax.InitToValue(      0); 
	nTotalMax.InitToValue(      0); 
	mTotalMax.InitToValue(      0); 

	for (int week=0; week<tDim; week++) {
		DcStatisticsStruct sWeekMin, sWeekMax, cWeekMin, cWeekMax, nWeekMin, nWeekMax, mWeekMin,mWeekMax;

		sWeekMin.InitToValue(1000000); // maximal value
		cWeekMin.InitToValue(1000000);
		nWeekMin.InitToValue(1000000);
		mWeekMin.InitToValue(1000000);

		sWeekMax.InitToValue(      0); // minimal value
		cWeekMax.InitToValue(      0); 
		nWeekMax.InitToValue(      0); 
		mWeekMax.InitToValue(      0); 

		for (int x=0;x<xDim;x++) {
			for (int y=0;y<yDim;y++) {
				for (int e=0;e<eDim;e++) {
					for (int a=0;a<aDim;a++) {
						for (int o=0; o<oDim; o++) {
							for (int s=0; s<sDim; s++) {

								char strat = _f_strat(x,y,e,a,o,s,week);
								
								// start
								if (strat=='s') {
									UpdateMinMaxStat(sTotalMin, sTotalMax, x,y,e,a,o,s,week);
									UpdateMinMaxStat(sWeekMin,  sWeekMax,  x,y,e,a,o,s,week);	
								}

								// care
								else if (strat=='c') {
									UpdateMinMaxStat(cTotalMin, cTotalMax, x,y,e,a,o,s,week);
									UpdateMinMaxStat(cWeekMin,  cWeekMax,  x,y,e,a,o,s,week);
								}

								// no care
								else if (strat=='n') {
									UpdateMinMaxStat(nTotalMin, nTotalMax, x,y,e,a,o,s,week);
									UpdateMinMaxStat(nWeekMin,  nWeekMax,  x,y,e,a,o,s,week);
								}

								// migrate
								else if (strat=='m') {
									UpdateMinMaxStat(mTotalMin, mTotalMax, x,y,e,a,o,s,week);
									UpdateMinMaxStat(mWeekMin,  mWeekMax,  x,y,e,a,o,s,week);		
								}
							}
						}
					}
				}
			}
		}

		if (sWeekMin.cnt==0) {
			sWeekMin.InitToValue(-1);
			sWeekMax.InitToValue(-1);
		}
		if (cWeekMin.cnt==0) {
			cWeekMin.InitToValue(-1);
			cWeekMax.InitToValue(-1);
		}
		if (nWeekMin.cnt==0) {
			nWeekMin.InitToValue(-1);
			nWeekMax.InitToValue(-1);
		}
		if (mWeekMin.cnt==0) {
			mWeekMin.InitToValue(-1);
			mWeekMax.InitToValue(-1);
		}

		if (weeklyOutput) {
			fprintf(file,"\n");

			if (nWeekMin.cnt!=0) {
				fprintf(file, " %2d  n=%6d  s=%6d  c=%6d  m=%6d \n\n", week, nWeekMin.cnt,sWeekMin.cnt,cWeekMin.cnt,mWeekMin.cnt);
			}
			if (sWeekMin.cnt!=0) {
				sWeekMin.PrintValues(file, "      start-min");
				sWeekMax.PrintValues(file, "      start-max");
			}
			if (cWeekMin.cnt!=0) {
				cWeekMin.PrintValues(file, "       care-min");
				cWeekMax.PrintValues(file, "       care-max");
			}
			if (cWeekMin.cnt!=0) {
				mWeekMin.PrintValues(file, "       migr-min");
				mWeekMax.PrintValues(file, "       migr-max");
			}
		}
	}



	if (sTotalMin.cnt==0) {
		sTotalMin.InitToValue(-1);
		sTotalMax.InitToValue(-1);
	}
	if (cTotalMin.cnt==0) {
		cTotalMin.InitToValue(-1);
		cTotalMax.InitToValue(-1);
	}
	if (nTotalMin.cnt==0) {
		nTotalMin.InitToValue(-1);
		nTotalMax.InitToValue(-1);
	}
	if (mTotalMin.cnt==0) {
		mTotalMin.InitToValue(-1);
		mTotalMax.InitToValue(-1);
	}

	fprintf(file, "========================================================== \n" );
	fprintf(file, "nocare: \n" );
	nTotalMin.PrintValues(file, "min",true);	
	nTotalMax.PrintValues(file, "max",true);
	fprintf(file, "----------------------------------------------------------- \n" );
	fprintf(file, "start: \n" );
	sTotalMin.PrintValues(file, "min",true);	
	sTotalMax.PrintValues(file, "max",true);
	fprintf(file, "----------------------------------------------------------- \n" );
	fprintf(file, "care: \n" );
	cTotalMin.PrintValues(file, "min",true);	
	cTotalMax.PrintValues(file, "max",true);
	fprintf(file, "----------------------------------------------------------- \n" );
	fprintf(file, "migrate: \n" );
	mTotalMin.PrintValues(file, "min",true);	
	mTotalMax.PrintValues(file, "max",true);
	fprintf(file, "========================================================== \n" );
}

void Decision::SaveStatesToBinaryFiles(char *fileprefix)
{
	char filename_f[FILENAME_MAX];
	char filename_fu[FILENAME_MAX];	
	char filename_fs[FILENAME_MAX];

	sprintf_s(filename_f, "%s_f_BW_%03d.bin", fileprefix, _year);
	sprintf_s(filename_fu,"%s_f_u_BW_%03d.bin", fileprefix, _year);
	sprintf_s(filename_fs,"%s_f_s_BW_%03d.bin", fileprefix, _year);

	FILE *file_f = fopen(filename_f,"wb");
	if (file_f==0) {
		printf("Decision::SaveStatesToBinaryFiles(%s) error opening the 'f' file for writing\n",filename_f);
		return;
	}
	else {
		_f.SaveBinary(file_f);
		fclose(file_f);
	}

	FILE *file_fu = fopen(filename_fu,"wb");
	if (file_fu==0) {
		printf("Decision::SaveStatesToBinaryFiles(%s) error opening the 'f_u' file for writing\n",filename_fu);
		return;
	}
	else {
		_f_u.SaveBinary(file_fu);
		fclose(file_fu);
	}


	FILE *file_fs = fopen(filename_fs,"wb");
	if (file_fs==0) {
		printf("Decision::SaveStatesToBinaryFiles(%s) error opening the 'f_s' file for writing\n",filename_fs);
		return;
	}
	else {
		_f_strat.SaveBinary(file_fs);
		fclose(file_fs);
	}
}
