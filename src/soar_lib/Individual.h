/**
* \file Individual.h
* \brief Declaration of class Individual
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

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>


/**
 * \ingroup SoarLib
 * \brief Stores information of each animal in the forward simulation
 * <HR />
 *
 * <H2 class="groupheader">Include</H2>
 */
class Individual
{
public:
    Individual(double x_, int x_i, double y_, int y_i, int e_, int a_, int o_, int s_, int t_, int _year);   //constructor
    ~Individual(void);  //destructor
        
    //functions to access private variables
    double	GetX(void) const {return _x;}
    double	GetY(void) const {return _y;}
    int		GetXindex(void) const {return _x_index;}
    int		GetYindex(void) const {return _y_index;}
    int		GetE(void) const {return _e;}
    int		GetA(void) const {return _a;}
	int		GetO(void) const {return _o;}
	int		GetS(void) const {return _s;}
    
    unsigned int GetID(void) const {return _identity;}
    bool	     IsAlive(void) const {return _alive;}
    unsigned int GetBirthWeek(void) const {return _birth_week;}
    unsigned int GetBirthYear(void) const {return _birth_year;}
    bool		 IsDeadCond(void) const	{return _dead_cond;}
    bool		 IsDeadRes(void) const {return _dead_res;}
    bool		 IsDeadDisease(void) const {return _dead_disease;}
    bool		 IsDeadPredation(void) const {return _dead_predation;}
    int			 GetDeathweek(void) const {return _death_week;}
    int			 GetDeathyear(void) const {return _death_year;}
    unsigned int GetDeath_brood(void) const {return _death__brood;}
	
	std::vector<int> GetTMigrstart(void) const {return _t_migr_start;}
    std::vector<int> GetTbroodstart(void) const {return _t_brood_start;}
    std::vector<int> GetNrKids(void) const {return _nr_kids;}
    
    // functions to manipulate private variables
    void SetX(double x) {_x = x;}
    void SetY(double y) {_y = y;}
    void SetXindex(int x) {_x_index = x;}
    void SetYindex(int y) {_y_index = y;}
    void Mature(void) {_e++;}
    void SetE(int e) {_e = e;}
    void SetA(int a) {_a = a;}
    void SetO(int o) {_o = o;}
    void SetS(int s) {_s = s;}
    void IncreaseS(void) {_s++;}

    void Age(void) {_age++;}
    void AgeBrood(void) {_a++;}
    void DieRes(int t, int year);
    void DieCond(int t, int year);
    void DieDisease(int t, int year);
    void DiePredation(int t, int year);
    void AbandonBrood() { _death__brood++; _a = -1;}
	void MigrStart(int t) {_s = 0; _t_migr_start.push_back(t);}    // ?? Why/Effect of s=0, a=0 (below) ??
    void BroodStart(int t) {_a = 0; _t_brood_start.push_back(t);}
    void IndependentBrood(int n, int t) {_t_fledging.push_back(t); _nr_kids.push_back(n); _nr_kids_life += n; } 

private:
    static int _popsize; //total number of individuals in simulation

    unsigned int _identity;
    
    // state variables from backward
    double _x;
    double _y;
    int _x_index;
    int _y_index;
    int _e;
    int _a;
	int _o; 
	int _s;
    
    // additional attributes
    unsigned int _birth_week;  // this is actually the fledging date
    unsigned int _birth_year;
    unsigned int _age;
    bool _alive;
    bool _dead_res;
    bool _dead_cond;
    bool _dead_disease;
    bool _dead_predation;
    int _death_week;
    int _death_year;
    unsigned int _death__brood;
	std::vector<int> _t_migr_start;
    std::vector<int> _t_brood_start;
    std::vector<int> _nr_kids;
    std::vector<int> _t_fledging;
    unsigned int _nr_kids_life;
};

#endif
