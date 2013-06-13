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

#ifndef MATLABINTERFACE_H
#define	MATLABINTERFACE_H

#include <stdio.h>  /* defines FILENAME_MAX */


#include <iostream>
#include <sstream>
#include <vector>

#include "engine.h"

#include <boost/filesystem.hpp>

#include "LoadFlowInterface.h"
#include "../utility/Utility.h"

#define BUFSIZE 256

// No need to include helper struct in documentation
/** \cond HIDDEN SYMBOLS */

// Each block port is a pair of block(matlab component) and one of its ports.
// Each model line in matlab connects a number of block-port pairs.
struct blockPort {
    std::string block;
    std::string port;
};

/** \endcond */


/** The interface to MATLAB SimPowerSystems.  The network model is built within
  * MATLAB, then imported into POSSIM here.  Vehicles are added to the MATLAB
  * model as required on the fly.  MATLAB takes care of all load flow
  * calculations. */
class MatlabInterface : public LoadFlowInterface {
    
private:
    /** The MATLAB Engine */
    Engine *eng;
    
    /** Necessary to retrieve outputs from MATLAB */
    mxArray *result;
    
    /** Local pointer to config */
    Config *config;

    /** Path to directory containing network model. */
    std::string modelDir;
    
    /** Name of network model. */
    std::string modelName;
    
    /** Name of network model without extension. */
    std::string modelNameRoot;
    
    /** Name of full path and model. */
    std::string modelNameFullPath;
        
    /** Name of network model library. */
    std::string modelLibName;
    
    /** Name of network model library without extension. */
    std::string modelLibNameRoot;
    
    /** Name of full path and model library. */
    std::string modelLibNameFullPath;
        
    /** For convenience of string interaction between POSSIM and MATLAB. */
    std::stringstream ss;
    
public:
    /** Constructor */
    MatlabInterface(Config* config);
    
    /** Destructor */
    ~MatlabInterface();

    /** Set directory in MATLAB where model sits. */
    void setDir(std::string dir);
    
    /** Load model that was built in MATLAB. */
    void loadModel(Config* config);
    
    /** Parse the matlab model.  Read full network structure and any relevant
      * information for individual components.*/
    void extractModel(FeederPole* &root, 
                      DistributionTransformer* &transformer,
                      std::map<std::string,FeederPole*> &poles, 
                      std::map<std::string,FeederLineSegment*> &lineSegments, 
                      std::map<std::string,Household*> &households);
    

    /** Run MATLAB load flow calculation. */
    void runSim();
    
    /** Get value of given variable in MATLAB */
    double getVar(std::string var);
    
    /** Set value of given component in MATLAB */
    void setVar(std::string component, double value, std::string var);
    
    /** Set value of given component in MATLAB */
    void setVar(std::string component, std::string value, std::string var);
    
    /** Set transformer capacity */
    void setTxCapacity(std::string component, double value);
    
    /** Get number of houses */
    int getNumHouses();
    
    /** Get house names */
    std::vector <std::string> getHouseNames();
    
    /** Add vehicle to MATLAB model. */
    void addVehicle(Vehicle vehicle);
    
    /** Set demand of given component. */
    void setDemand(std::string component, double a, double i, double c);
    
    /** Set demand of components specified in give file. */
    void setDemand(std::string filename);
    
    /** Print network model (ideally to PDF in simulation log directory) */
    void printModel(std::string targetDir);
    
    /** Generate report. */
    void generateReport(std::string dir, int month, bool isWeekday, int simInterval);
    
    /** Get MATLAB load flow output following load flow calculation. */
    void getOutputs(double phaseV[12], double phaseI[12], double eolV[12], std::map<std::string, Household*> &households);
    
    /** Run Optimisation in matlab.*/
    void runOptimisation(std::string optDir, std::string optAlg, 
                         int numDecVars, int numConstraints, 
                         double &fval, double &exitflag);
    
    
private:
    /** Extract information about line segments, houses, and connections between 
      * blocks from raw matlab model file */
    void parseModelFile(DistributionTransformer* &transformer,
                        std::map<std::string,FeederLineSegment*> &lineSegments, 
                        std::map<std::string,Household*> &households,
                        std::vector< std::vector<blockPort>  > &modelLines);
    
    /** As part of network tree building process, create the root of the tree */
    void createRoot(FeederPole* &root,
                    std::map<std::string,Household*> &households,
                    std::map<std::string,FeederLineSegment*> &lineSegments,
                    std::vector< std::vector<blockPort> > modelLines);

    void buildTree(FeederLineSegment* &root,
                   std::map<std::string,FeederPole*> &poles,
                   std::map<std::string,FeederLineSegment*> &lineSegments,
                   std::map<std::string,Household*> &households,
                   std::vector< std::vector<blockPort> > modelLines);
};

#endif	/* MATLABINTERFACE_H */

