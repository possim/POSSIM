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

#include "Utility.h"

using namespace boost::posix_time;
    
// Return time difference between two time points, in milliseconds
long utility::timeDiff(ptime t1, ptime t2){       
    time_duration td = t2 - t1;
    return (long)(td.total_milliseconds());
} 

// Return time since start in milliseconds
long utility::timeSince(ptime start) {
    boost::posix_time::time_duration duration = boost::posix_time::microsec_clock::local_time() - start;
    return((long)(duration.total_milliseconds()));
}

std::string utility::time2string(int n) {
    std::ostringstream result;
    if(n<10) result << "0";
    result << n;
    return result.str();
}

// Display time according to how much is left
std::string utility::timeDisplay(long time) {
    int numHrs, numMins, numSecs, numMsecs;
    std::stringstream ss;
    
    numHrs   = time / 3600000;
    numMins  = (time % 3600000) / 60000;
    numSecs  = (time % 60000) / 1000;
    numMsecs = time % 1000;
    
    if(time > 60000)
        ss << numHrs << "h " << numMins << "m";
    else if(time > 1000)
        ss << numMins << "m " << numSecs << "s";
    else
        ss << numMsecs << "ms";
    return ss.str();
}

std::string utility::int2string(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

int utility::string2int(std::string s) {
    int result;
    std::stringstream ss(s);
    if(!(ss >> result))
        return 0;
    else
        return result;
}

double utility::string2double(std::string s) {
    double result;
    std::stringstream ss(s);
    if(!(ss >> result))
        return 0;
    else
        return result;
}

void utility::tokenize(const std::string& str, std::vector<std::string>& tokens, std::string del)
{
    std::string delimiters = del;
 
	// Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
 
    tokens.clear();
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

// Implements Fisher-Yates shuffle 
//(see http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
// and http://ideone.com/3A3cv)
std::vector<int>::iterator utility::random_unique(std::vector<int>::iterator begin, std::vector<int>::iterator end, size_t num_random) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        std::vector<int>::iterator r = begin;
        std::advance(r, rand()%left);
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}

// Returns random number according to random distribution having mean and standard dev sigma
// values above max and below min are set to mean
double utility::randomNormal(double mean, double sigma, double min, double max) {
    typedef boost::mt19937                     ENG;    // Mersenne Twister
    typedef boost::normal_distribution<double> DIST;   // Normal Distribution
    typedef boost::variate_generator<ENG,DIST> GEN;    // Variate generator
    struct timeval timemilli;
    gettimeofday(&timemilli, NULL);
    ENG  eng(timemilli.tv_usec);
    DIST dist(mean,sigma);
    GEN  gen(eng,dist);
 
    double curr = gen();
    if(curr < min || curr > max) curr = mean;

    return curr;
}

double utility::randomNormal(double *vars) {
    return randomNormal(vars[0], vars[1], vars[2], vars[3]);
}


double utility::randomUniform() {
    return (double)rand()/RAND_MAX;
}

// Return uniform random number, scaled to range (min, max)
double utility::randomUniform(double min, double max) {
    double f = (double)rand()/RAND_MAX;
    return min + f * (max-min);
}

// Return power factor given scalar V, I
double utility::calcPowerFactor(double phaseV, double phaseI) {
    return fabs(cos(M_PI/180 * (phaseV-phaseI)));
}