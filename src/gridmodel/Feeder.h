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


#ifndef FEEDER_H
#define	FEEDER_H

#include <string>
#include <vector>
#include <iostream>

#include "../networkgraph/NetworkGraphComponents.h"
#include "../household/Household.h"


/* Forward declarations */
class FeederLineSegment;
class Household;

/** Simple struct to store a power line's length and impedance */
struct lineModel {
    double resistance;
    double inductance;
    double capacitance;
    double length;
    double totalResistance() {
        return (length * resistance);
    }
    double totalReactance() {
        return (length * (inductance-capacitance));
    }
};


/** Simulates a single pole in the distribution network. A pole has one parent
  * feeder line, any number of child feeder lines, and any number of houses
  * connected to it. */
class FeederPole : public NetworkGraphVertex {

public:
    /** The name of this pole. Usually similar to its parent line segment. */
    std::string name;
    
    /** The parent feeder line segment of this pole. */
    FeederLineSegment* parentLineSegment;
    
    /** A vector of all child line segments of this pole.*/
    std::vector<FeederLineSegment*> childLineSegments;
    
    /** A vector of all houses connected to this pole. */
    std::vector<Household*> households;
    

public:
    FeederPole() {
        name = "Noname";
    };

    ~FeederPole() {
        //delete parentLineSegment;
        //delete[] childLineSegments;
    };
};


/** Simulates a single distribution line segment between two poles in the 
  * distribution network.  Stores impedance and length of the segment, as well  
  * as pointers to its parent and child poles. */
class FeederLineSegment : public NetworkGraphEdge {

public:
    /** Name of feeder line segment.  Should match the equivalent segment
      * in the MATLAB model (or whatever load flow software the model was build
      * in). */
    std::string name;
    
public:
    lineModel line;
    
    FeederPole *parentPole;
    FeederPole *childPole;
    
    std::string parentPoleName;
    std::string childPoleName;

public:
    FeederLineSegment() {
        name = "Noname";
        line.resistance = 0.0;
        line.inductance = 0.0;
        line.capacitance = 0.0;
        line.length = 0.0;
    };
    
    FeederLineSegment(std::string n, double r, double i, double c, double l) {
        set(n, r, i, c, l);
    };
    
    void set(std::string n, double r, double i, double c, double l) {
        name = n;
        line.resistance = r;
        line.inductance = i;
        line.capacitance = c;
        line.length = l;
    };

    ~FeederLineSegment() {
        //delete parentPole;
        //delete childPole;
    };

};

#endif	/* FEEDER_H */

