/* 
Software License Agreement (BSD License)

Copyright (c) 2013, Julian de Hoog <julian@dehoog.ca>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution.
 * The name of the author may not be used to endorse or promote 
   products derived from this software without specific prior 
   written permission from the author.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE. 
*/

#ifndef DATETIME_H
#define	DATETIME_H



#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "Utility.h"


class DateTime {

public:
    int hour;           // hour, 0-23
    int minute;         // minutes, 0-59
    int day;            // day in current month, 1-31
    int month;          // which month 1-12
    int year;           // 1990, 1991, ...
    int weekday;        // which day of week 1=Monday, 7=Sunday
    
    DateTime();
    DateTime(const char* time);
    virtual ~DateTime();
    
    void tokenize(const char* datetime);
    void set(int newHour, int newMinute, int newDay, int newMonth, int newYear);
    void set(std::string newDateTime);
    void setTime(int mins);

    bool isEarlierThan(DateTime other);
    bool equals(DateTime other);
    bool isLaterThan(DateTime other);
    
    bool isEarlierInDayThan(DateTime other);    // ignores day, month, year
    bool isLaterInDayThan(DateTime other);    // ignores day, month, year
    
    bool isWeekday();
    bool isMidnight();
    
    void increment(int minutes);
    
    int  totalMinutes();
    int  diffInMinutes(DateTime other);     // ignores day, month, year
    long minus(DateTime other, int numMins); // returns num intervals difference to other
    
    void display();
    std::string toString();
    std::string formattedString();
};

#endif	/* DATETIME_H */

