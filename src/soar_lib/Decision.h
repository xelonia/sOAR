/**
* \file Decision.h
* \brief Declaration of class Decision
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

#ifndef DECISION_H
#define DECISION_H

#include "..\soar_support_lib\NArray.h"

/**
 * \ingroup SoarLib
 * \brief Stores the 7 dimensional simulation states
 * <HR />
 *
 * <H2 class="groupheader">Include and Collaboration</H2>
 */
class Decision {
private:
	bool	_initialized;		///< Flags if the state arrays need initialization

	NArray<double>	_f;			///< State array for reproductive value
	NArray<double>	_f_u;		///< State array for optimal foraging intensity
	NArray<char>	_f_strat;	///< State array for optimal behavioral decision (n=no care, c=care, s=start, m=migrate)

	double	_lambda;			///< Todo: Describe purpose
	double  _lambda_avg;		///< Todo: Describe purpose		
	double  _lambda_state;		///< Todo: Describe purpose
	double  _lambda_worst;	    ///< Todo: Describe purpose		
	int     _bw_not_converged;  ///< Todo: Describe purpose		
	int     _bw_state_count;    ///< Todo: Describe purpose		
	double	_theta;				///< Todo: Describe purpose
    bool	_converged;			///< Todo: Describe purpose
    double	_conv_level_max;	///< Todo: Describe purpose
    double	_conv_level_sum;	///< Todo: Describe purpose
	int     _year;				///< Total number of periods in backward iteration


	/**
	* Outputs statistics summary to a file in human readable form.
	* \param file          Filepointer
	* \param weeklyOutput  Toggles output of weekly data (epochs)
	*/
	void WriteSummaryStatistics(FILE *file, bool weeklyOutput);

	/// \brief Structure to sample statics about min/max values of the decision states 
	struct DcStatisticsStruct {
		int x,y,e,a,o,s,t, cnt;

		void InitToValue(int val) { x = y = e = a = s = o = s = t = val; cnt=0; }

		void PrintValues(FILE *file, char *header, bool showTotal=false) {
			if (showTotal)
				fprintf(file, "%s %6d  %6d  %6d  %6d  %6d  %6d  %6d\n",header, x,y,e,a,o,s,t);
			else
				fprintf(file, "%s:   x=%2d  y=%2d  e=%2d  a=%2d  o=%2d  s=%2d\n",header, x,y,e,a,o,s);
		}
	};

	/// \brief Updates two DcStatisticsStruct for min and max values with the current values
	void UpdateMinMaxStat(DcStatisticsStruct &min, DcStatisticsStruct &max, 
		int x, int y, int e, int a, int o, int s, int t=0);

public:
	/// \brief Constructor
	Decision();
	/// \brief Destructor
	~Decision();

	/// \brief Resets the decision into 'uninitialized' state
	void Reset();

	/// \brief Initializes the state arrays to given dimensions
	void InitDimensions(int xDim, int yDim, int eDim, int aDim, int oDim, int sDim, int tDim);


	/// \brief Returns TRUE if the state arrays are allocated
	bool IsInitialized()	{	return _initialized;	}


	/// \name State array access
	/// @{ 
	/// \brief Return reference to f array object
	NArray<double> & GetF()	{	return _f;	};

	/// \brief Sets the reproductive value for a given state vector 
	void   SetF(int x, int y, int e, int a, int o, int s, int t, double value);
	/// \brief Returns the reproductive value for a given state vector 
	double GetF(int x, int y, int e, int a, int o, int s, int t);

	/// \brief Sets the optimal foraging intensity for a given state vector 
	void   SetF_u(int x, int y, int e, int a, int o, int s, int t, double value);
	/// \brief Returns the optimal foraging intensity for a given state vector 
	double GetF_u(int x, int y, int e, int a, int o, int s, int t);

	/// \brief Sets the optimal behavioral decision for a given state vector 
	void   SetF_strat(int x, int y, int e, int a, int o, int s, int t, char value);
	/// \brief Returns the optimal behavioral decision for a given state vector 
	char   GetF_strat(int x, int y, int e, int a, int o, int s, int t);

	/// \brief Sets all three values (reproductive value,optimal foraging intensity, optimal behavioral decision) for a given state vector 
	void   SetF_all(int x, int y, int e, int a, int o, int s, int t, double f, double f_u, char f_strat);
	///@} End of group started by \name


	void   SetLambda(double l)			{	_lambda = l;				}
	double GetLambda()					{	return _lambda;				}

	void   SetLambdaState(double l)		{	_lambda_state = l;			}
	double GetLambdaState()				{	return _lambda_state;		}

	void   SetLambdaWorst(double l)     {   _lambda_worst = l;          }	
	double GetLambdaWorst()				{	return _lambda_worst;		}	
		
	void   SetBwNotConvCount(int i)     {   _bw_not_converged = i;      }	
	int    GetBwNotConvCount()          {   return _bw_not_converged;   }	

	void   SetBwStateCount(int i)       {   _bw_state_count = i;        }	
	int    GetBwStateCount()            {   return _bw_state_count;     }		

	void   SetTheta(double t)			{	_theta = t;					}
	double GetTheta()					{	return _theta;				}
    
    void   SetConvergence(bool co)		{   _converged = co;			}
    bool   GetConvergence()				{   return _converged;			}
    
    void   SetConvMax(double co)		{	_conv_level_max = co;		}
    double GetConvMax()					{	return _conv_level_max;		}
    
	void   SetConvSum(double co)		{	_conv_level_sum = co;		}
    double GetConvSum()					{	return _conv_level_sum;		}

	void   SetYear(int y)				{	_year = y;					}
	int    GetYear()					{	return _year;				}


	/// \name Saving and loading of decision state
	/// @{ 
	/**
	 * \brief Save all dimensions and arrays into a single binary file; 
	 * \return True if succesfull, else save error
	 */
	bool SaveToFile(char *filename);

	/**
	 * \brief Load all dimensions and arrays from a single binary file; 
	 * \return True if succesfull, else loading error
	 */
	bool LoadFromFile(char *filename);
	///@} End of group started by \name


	/// \name Statistics output
	/// @{ 
	/// \brief Saves the summary statistics into an ascii file
	void SaveSummaryStatisticsToFile(char *filename);

	/// \brief Prints a summary to the console 
	void PrintSummary();

	/// \brief Prints summary statistics to the console 
	void PrintSummaryStatistics();

	/// \brief Output binaries data files for the three state arrays
	void SaveStatesToBinaryFiles(char *fileprefix);

	///@} End of group started by \name
};

#endif // DECISION_H