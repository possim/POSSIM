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

#include "SpotPrice.h"

SpotPrice::SpotPrice(Config* config) {
    std::cout << "Loading spot price data... ";
    std::cout.flush();
    
    simInterval = config->getInt("simulationinterval");
    dataDir = config->getString("spotpricedata");
    price = 0;
        
    std::cout << "OK" << std::endl;
    std::cout.flush();
}

SpotPrice::SpotPrice(std::string dirName) {
    dataDir = dirName;
}

SpotPrice::~SpotPrice() {
}

double SpotPrice::findPriceAt(DateTime datetime) {
    // Generate correct filename
    std::stringstream ss;
    ss << dataDir << "DATA" << datetime.year;
    if(datetime.month < 10) ss << "0";
    ss << datetime.month << "_VIC1.csv";
    
    // Open data file
   std::fstream infile(ss.str().c_str(), std::ios::in);
   if(!infile){
      std::cout << "Cannot open spot price input file.";
      exit (1);
   }
   
   // Skip to correct line
   int skiplines = 1 + (datetime.day - 1)*48 + (datetime.totalMinutes()/30);
   std::string line;
   for(int i=0; i<skiplines; i++)
      getline(infile, line);
   
   // Skip to correct token
   std::istringstream tokenStream(line);
   std::string token;
   std::vector<std::string> alltokens;
   while(getline(tokenStream, token, ','))
       alltokens.push_back(token);

   infile.close();
    
    return strtod(alltokens.at(3).c_str(), NULL);
}

void SpotPrice::update(DateTime datetime) {
    price = findPriceAt(datetime);
}

void SpotPrice::display() {
    std::cout << "Spot price      : " << std::setw(7) << std::right << std::setiosflags(std::ios::fixed) << std::setprecision(2) << price << std::endl;
}