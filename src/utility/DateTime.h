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

/** A simple date and time class to keep track of simulation intervals, ensure
  * simulation start / end, etc.*/
class DateTime {

public:
    int hour;           // hour, 0-23
    int minute;         // minutes, 0-59
    int day;            // day in current month, 1-31
    int month;          // which month 1-12
    int year;           // 1990, 1991, ...
    int weekday;        // which day of week 1=Monday, 7=Sunday
    
    /** Constructor, without arguments is set to 00:00:00 on 1 January 2012. */
    DateTime();
    
    /** Constructor with string-based argument */
    DateTime(const char* time);
    
    /** Destructor*/
    virtual ~DateTime();
    
    /** Reset date time using arguments */
    void set(int newHour, int newMinute, int newDay, int newMonth, int newYear);

    /** Reset date time using string */
    void set(std::string newDateTime);
    
    /** Reset time only using total mins elapsed of full day */
    void setTime(int mins);

    /** True if this DateTime is earlier than other */
    bool isEarlierThan(DateTime other);
    
    /** True if this DateTime equals other (date and time) */
    bool equals(DateTime other);
    
    /** True if this DateTime is later than other */
    bool isLaterThan(DateTime other);
    
    /** True if time only is earlier than other */
    bool isEarlierInDayThan(DateTime other);
    
    /** True if time only is later than other*/
    bool isLaterInDayThan(DateTime other);
    
    /** True if not Saturday or Sunday */
    bool isWeekday();
    
    /** True if midnight */
    bool isMidnight();
    
    /** Increments time by specified number of minutes */
    void increment(int minutes);
    
    /** Returns total minutes passed in this day since midnight */
    int  totalMinutes();
    
    /** Returns difference to other in minutes only (only time, ignores date) */
    int  diffInMinutes(DateTime other);
    
    /** Returns number of intervals difference to other, using specified interval size */
    long minus(DateTime other, int numMins);
    
    /** Displays full date and time */
    void display();
    
    /** Returns string value of full date and time */
    std::string toString();
    
    /** Returns formatted string of full date and time*/
    std::string formattedString();

private:
    /** Turn string-based time into DateTime object */
    void tokenize(const char* datetime);
};

#endif	/* DATETIME_H */

