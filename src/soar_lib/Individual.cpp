/**
* \file Individual.cpp
* \brief Implementation of class Individual
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
#include "Individual.h"


int Individual::_popsize=0;	  // initialise number of Individuals


//-------------
// constructor

Individual::Individual(double x_, int x_i, double y_, int y_i, int e_, int a_, int o_, int s_, int t_, int _year)
{
    _popsize++;
    _identity = _popsize;
    _x = x_;
    _x_index = x_i;
    _y = y_;
    _y_index = y_i;
    _e = e_;
    _a = a_;
	_o = o_;
	_s = s_;
    
    _birth_week = t_;
    _birth_year = _year;
    _alive = true;
    _age = 0;
    
    _dead_res = false;
    _dead_cond = false;
    _dead_disease = false;
    _dead_predation = false;
    _death_week = -1;
    _death_year = -1;
    _death__brood = 0;
    
    _nr_kids_life = 0;    
}


//-------------
// destructor

Individual::~Individual(void)
{
    
}

//------------
void Individual::DieRes(int t, int year)
{
    _dead_res = true; 
	_alive = false; 
	_death_week = t; 
	_death_year = year;
    if (_a > -1) 
		_death__brood++;
}

void Individual::DieCond(int t, int year)
{
    _dead_cond = true; 
	_alive = false; 
	_death_week = t; 
	_death_year = year;
    if (_a > -1) 
		_death__brood++;
}

void Individual::DieDisease(int t, int year)
{
    _dead_disease = true; 
	_alive = false; 
	_death_week = t; 
	_death_year = year;
    if (_a > -1) 
		_death__brood++;
}

void Individual::DiePredation(int t, int year)
{
    _dead_predation = true; 
	_alive = false; 
	_death_week = t; 
	_death_year = year;
    if (_a > -1) 
		_death__brood++;
}

