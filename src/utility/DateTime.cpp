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

#include "DateTime.h"

int findDayOfWeek(int date, int month, int year);


DateTime::DateTime() {
    hour = 0;
    minute = 0;
    day = 1;
    month = 1;
    year = 2012;
    weekday = findDayOfWeek(day, month, year);
}

DateTime::DateTime(const char* time) {
    tokenize(time);
}

DateTime::~DateTime() {
}

void DateTime::set(int newHour, int newMinute, int newDay, int newMonth, int newYear) {
    hour = newHour;
    minute = newMinute;
    day = newDay;
    month = newMonth;
    year = newYear;
    weekday = findDayOfWeek(day, month, year);
}

void DateTime::set(std::string newDateTime) {
    tokenize(newDateTime.c_str());
}

void DateTime::setTime(int mins) {
    hour = mins/60;
    minute = mins%60;
}

void DateTime::tokenize(const char* dateTime) {
    std::vector<std::string> tokens;
    utility::tokenize(dateTime,tokens,".");
    try {
        hour = atoi(tokens.at(0).c_str());
        minute = atoi(tokens.at(1).c_str());
        day = atoi(tokens.at(2).c_str());
        month = atoi(tokens.at(3).c_str());
        year = atoi(tokens.at(4).c_str());
        weekday = findDayOfWeek(day, month, year);
    }
    catch(std::out_of_range& e) {
        std::cout << "Sorry, couldn't parse your string!" << std::endl;
        hour = 0;
        minute = 0;
        day = 1;
        month = 1;
        year = 2012;
        weekday = findDayOfWeek(day, month, year);
    }
}

bool DateTime::isEarlierThan(DateTime other) {
    if(year > other.year)
        return false;
    if(year == other.year){
       if(month > other.month)
           return false;
       if(month == other.month) {
           if(day > other.day)
               return false;
           if(day == other.day) {
               if(hour > other.hour)
                   return false;
               if(hour == other.hour)
                   if(minute >= other.minute)
                       return false;
           }
       }
    }
        
    return true;
}

bool DateTime::equals(DateTime other) {
    if(year == other.year &&
       month == other.month &&
       day == other.day &&
       hour == other.hour &&
       minute == other.minute)
        return true;
    else
        return false;
}

bool DateTime::isLaterThan(DateTime other) {
    if(isEarlierThan(other) || equals(other))
        return false;
    else  
        return true;
}

bool DateTime::isEarlierInDayThan(DateTime other) {
    if(totalMinutes() < other.totalMinutes())
        return true;
    
    return false;
}

bool DateTime::isLaterInDayThan(DateTime other) {
    if(totalMinutes() > other.totalMinutes())
        return true;
    
    return false;
}


bool DateTime::isWeekday() {
    return (weekday < 6);
}

bool DateTime::isMidnight() {
    if(hour == 0 && minute == 0)
        return true;
    return false;
}

std::string weekDayName(int weekday) {
    std::string weekDayName[7] = {
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat",
        "Sun"
    };
    return weekDayName[weekday-1];
}

std::string monthName(int month) {
    std::string monthName[12] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };   
    return monthName[month-1];
}

bool isLeapYear(int year) {
    bool retval = false;
    if (year % 100 == 0) {
        if (year % 400 == 0) {
            retval = true;
        }
    } else if (year % 4 == 0) {
        retval = true;
    }
    return retval;
}

int daysInMonth(int month, int year) {
    switch (month) {
        case 4:
        case 6:
        case 9:
        case 11:
            return 30;
            break;
        case 2:
            if (isLeapYear(year)) {
                return 29;
            } else return 28;
            break;
        default:
            return 31;
            break;
    }
}

int findDayOfWeek(int date, int month, int year) {
    int refYear = 1990;
    int refWeekDay = 1; // Jan 1, 1990 was a Monday
    
    if(year < 1990)
        std::cerr << "ERROR: Year chosen earlier than reference year!\n";
    
    int numDays = 0;
    for(int i=refYear; i<year; i++)
        if(isLeapYear(i)) 
            numDays += 366;
        else
            numDays += 365;
    for(int j=1; j<month; j++)
        numDays += daysInMonth(j, year);
    numDays += date;
    
    return((numDays - refWeekDay)%7 + 1);
}


void DateTime::increment(int numMinutes) {
    minute += numMinutes;
    
    while (minute >= 60) {
        minute-= 60;
        hour++;
    }
    while(hour >= 24) {
        hour -= 24;
        weekday ++;
        day++;
    }
    while(weekday > 7)
        weekday -= 7;

    if(day > daysInMonth(month, year)) {
        day = 1;
        month++;
        if(month > 12) {
            month = 1;
            year++;
        }
    }
}

long DateTime::minus(DateTime other, int numMins) {
    long numIntervals = 0;
    
    while(isLaterThan(other)) {
        numIntervals++;
        other.increment(numMins);
    }
    
    return(numIntervals); 
}

int DateTime::diffInMinutes(DateTime other) {
    return(std::abs(totalMinutes() - other.totalMinutes()));
}

int DateTime::totalMinutes() {
    return (hour*60 + minute);
}

void DateTime::display() {
    std::cout << toString() << std::endl;
}

std::string DateTime::toString() {
    std::stringstream ss;
    ss << year << " "
       << monthName(month) << " "
       << day << " ("
       << weekDayName(weekday) << ") "
       << hour << ":";
    if(minute < 10) ss << "0";
    ss << minute;
    
    return ss.str();
}

std::string DateTime::formattedString() {
    std::stringstream ss;
    if(hour < 10) ss << "0";
    ss << hour << ".";
    if(minute < 10) ss << "0";
    ss << minute << ".";
    if(day < 10) ss << "0";
    ss << day << ".";
    if(month < 10) ss << "0";
    ss << month << ".";
    ss << year;
    
    return ss.str();
}


