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



#include "MatlabInterface.h"



/*   CONSTRUCTOR, DESTRUCTOR   */
    
MatlabInterface::MatlabInterface(Config* cfg) {
    // Store local pointer to config
    config = cfg;
    
    // START Matlab
    std::cout << " - Trying to start MATLAB ...";
    std::cout.flush();
    if (!(eng = engOpen(""))) {
        std::cout << "Fail!" << std::endl;
        return;
    }
    std::cout << " OK" << std::endl;
    
    // Set working directory in matlab, add path to matlab directory
    boost::filesystem::path working_path(boost::filesystem::current_path());
    setDir(working_path.string());
        
    // Load model
    loadModel(config);
}

MatlabInterface::~MatlabInterface() {
    mxDestroyArray(result);
    engClose(eng);
}

void MatlabInterface::setDir(std::string dir) {
    ss.str("");
    ss << "cd " << dir << "; "
       << "addpath('" << dir << "/matlab');";
    engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::loadModel(Config* cfg) {
    config = cfg;
    
   // Set all names and paths for convenience
    modelNameFullPath = config->getString("modelpath");
    std::string::size_type pos=modelNameFullPath.find_last_of("/\\")+1;
    modelName = modelNameFullPath.substr(pos,modelNameFullPath.length()-pos);
    modelNameRoot = modelName.substr(0,modelName.find_last_of('.'));
    
    modelLibNameFullPath = config->getString("modellibpath");
    pos=modelLibNameFullPath.find_last_of("/\\")+1;
    modelLibName = modelLibNameFullPath.substr(pos,modelLibNameFullPath.length()-pos);
    modelLibNameRoot = modelLibName.substr(0,modelLibName.find_last_of('.'));

    ss.str("");
    ss << "systemHandle = load_system" << "('" << modelName << "')";
    
    std::cout << " - Loading model " << modelName << " ...";
    std::cout.flush();
    
    engEvalString(eng, ss.str().c_str());
    
    std::cout << " OK" << std::endl;
}

void MatlabInterface::parseModelFile(DistributionTransformer* &transformer, 
                                     std::map<std::string,FeederLineSegment*> &lineSegments, 
                                     std::map<std::string,Household*> &households,
                                     std::vector< std::vector<blockPort>  > &modelLines) {
    
   // Ensure both model and its library exist.
   std::ifstream modelFile(modelNameFullPath.c_str());
   if(!modelFile){
       std::cout << "Cannot open model file " << modelNameFullPath << ".  Does it exist?" << std::endl;
       exit (1);
   }
   std::ifstream modelLibraryFile(modelLibNameFullPath.c_str());
   if(!modelLibraryFile){
       std::cout << "Cannot open model library file " << modelLibNameFullPath << ".  Does it exist?" << std::endl;
       exit (1);
   }

   // Set house and line lib component names
   ss.str("");
   ss << "\"" << modelLibNameRoot << "/Individual House\"";
   std::string modelLibHouse = ss.str();
   ss.str("");
   ss << "\"" << modelLibNameRoot << "/Backbone\"";
   std::string modelLibBackbone = ss.str();

   // Some utility variables
   std::string line;
   std::vector<std::string> tokens;
   std::string::size_type pos1, pos2;
   std::string key, value;
   std::map<std::string, std::string> currBlock;
   blockPort thisBP;
   std::vector<blockPort> thisModelLine;
   bool isHouse, isPowerLine, isRoot, isTransformer, isBackboneModel, isServiceLineModel;
   mwSize bufferLength;
   mxArray *tempMxArray;
   char* componentName;
    
   // NMI counter
   int NMIcounter = 1;
   
   // Line models
   lineModel backbone, serviceline;
   
   // First, get model of lines (backbone and service) from library file
   while(getline(modelLibraryFile,line)) {
       utility::tokenize(line, tokens, "\t ");

       // If we are starting a new block
       if(tokens.size() == 2 && tokens.at(0) == "Block" && tokens.at(1) == "{") {
           currBlock.clear();
           isBackboneModel = false;
           isServiceLineModel = false;
       
           int depthCounter = 1;

           // Looping through all parts of this line including branches
           while(depthCounter > 0) {
                getline(modelLibraryFile,line);
                utility::tokenize(line, tokens, "\t ");
                
                // Keep track of depth
                if(tokens.size() == 2 && tokens.at(1) == "{")
                    depthCounter++;
                else if(tokens.size() == 1 && tokens.at(0) == "}")
                    depthCounter--;
                
                // Search all blocks
                else {
                    // Find key and value of this line
                    key = tokens.at(0);
                    pos1 = line.find_first_not_of("\t ", 0);
                    pos2     = line.find_first_of("\t", pos1);
                    pos1 = line.find_first_not_of("\t ", pos2+1);
                    value = line.substr(pos1, line.length()-pos1);
                           
                    // Is this a backbone or service line model?
                    if(key == "Name" && value == "\"Backbone\"")
                        isBackboneModel = true;
                    else if(key == "Name" &&
                            (value == "\"Service Active\"" || value == "\"Service_Active\""))
                        isServiceLineModel = true;
                    currBlock[key] = value;
                }
           }

           if(isBackboneModel) {
               backbone.resistance = utility::string2double(currBlock["Resistance"].substr(1,currBlock["Resistance"].length()-2));
               backbone.inductance = utility::string2double(currBlock["Inductance"].substr(1,currBlock["Inductance"].length()-2));
               backbone.capacitance = utility::string2double(currBlock["Capacitance"].substr(1,currBlock["Capacitance"].length()-2));
               std::cout << " - Found backbone line model, with resistance " << std::setprecision(4) << backbone.resistance 
                         << ", inductance " << std::setprecision(4) << backbone.inductance
                         << ", capacitance " << std::setprecision(4) << backbone.capacitance << std::endl;
           }
           else if(isServiceLineModel) {
               serviceline.resistance = utility::string2double(currBlock["Resistance"].substr(1,currBlock["Resistance"].length()-2));
               serviceline.inductance = utility::string2double(currBlock["Inductance"].substr(1,currBlock["Inductance"].length()-2));
               serviceline.capacitance = utility::string2double(currBlock["Capacitance"].substr(1,currBlock["Capacitance"].length()-2));
               std::cout << " - Found  service line model, with resistance " << std::setprecision(4) << serviceline.resistance 
                         << ", inductance " << std::setprecision(4) << serviceline.inductance
                         << ", capacitance " << std::setprecision(4) << serviceline.capacitance << std::endl;
           }
       }
   }
   
   // Second, read all blocks by looping through entire model file
   while(getline(modelFile,line)) {
       utility::tokenize(line, tokens, "\t "); 

       // If we are starting a new block
       if(tokens.size() == 2 && tokens.at(0) == "Block" && tokens.at(1) == "{") {
           currBlock.clear();
           isHouse = false;
           isRoot = false;
           isPowerLine = false;
           isTransformer = false;
           
           getline(modelFile,line);
           utility::tokenize(line, tokens, "\t ");
           
           // Loop until end of block
           while(!(tokens.size()<2 && tokens.at(0)=="}")) {
               
               // Find key and value of this line
               key = tokens.at(0);
               pos1 = line.find_first_not_of("\t ", 0);
               pos2     = line.find_first_of("\t ", pos1);
               pos1 = line.find_first_not_of("\t ", pos2+1);
               value = line.substr(pos1, line.length()-pos1);
               
               // Is this a house?  If so make sure to add it to houses
               if(key == "SourceBlock" && value == modelLibHouse)
                   isHouse = true;
               
               // Is this a power line?  If so make sure to add to powerLines
               else if(key == "SourceBlock" && value == modelLibBackbone)
                   isPowerLine = true;
               
               // Is this the distribution transformer?  If so store values
               else if(key == "SourceType" && (value == "\"Three-Phase Transformer (Two Windings)\""))
                   isTransformer = true;
               
               // Is this the tree root (input into distribution network)?
               else if(key == "Name" && (value=="\"PhaseA\"" || value=="\"PhaseB\"" || value=="\"PhaseC\"" || value=="\"Neutral\""))
                   isRoot = true;

               currBlock[key] = value;

               getline(modelFile,line);
               utility::tokenize(line, tokens, "\t ");
           }
          
           if(isHouse) {
               Household *newHouse = new Household();
               newHouse->name = utility::stripQuotations(currBlock["Name"]);
               newHouse->NMI = NMIcounter;
               NMIcounter++;
               newHouse->serviceLine.resistance = serviceline.resistance;
               newHouse->serviceLine.inductance = serviceline.inductance;
               newHouse->serviceLine.capacitance = serviceline.capacitance;
               newHouse->serviceLine.length = utility::string2double(utility::stripQuotations(currBlock["length"]));
               newHouse->hasParent = false;
               households[newHouse->name] = newHouse;
               
               // Need to also store handle to component in matlab model.  This is the lazy way...
               ss.str("");
               ss << "houseName = find_system('" << modelNameRoot << "', 'FollowLinks', 'on', 'LookUnderMasks', 'on', 'name', '" << newHouse->name << "');"; 
               engEvalString(eng, ss.str().c_str());
               result = engGetVariable(eng, "houseName");
               tempMxArray = mxGetCell(result, 0);
               bufferLength = mxGetNumberOfElements(tempMxArray) + 1;
               componentName = (char *)mxCalloc(bufferLength, sizeof(char));
               mxGetString(tempMxArray, componentName, bufferLength);
               newHouse->componentName = componentName;
               newHouse->componentName.append("/House");
               //std::cout << "New house has componentName: |" << newHouse->componentName << "|" << std::endl;
           }
           else if(isPowerLine) {
               FeederLineSegment *newFeederLineSegment = new FeederLineSegment();
               newFeederLineSegment->name = utility::stripQuotations(currBlock["Name"]);
               newFeederLineSegment->line.resistance = backbone.resistance;
               newFeederLineSegment->line.inductance = backbone.inductance;
               newFeederLineSegment->line.capacitance = backbone.capacitance;                              
               newFeederLineSegment->line.length = utility::string2double(utility::stripQuotations(currBlock["length"]));
               lineSegments[newFeederLineSegment->name] = newFeederLineSegment;
           }
           else if(isTransformer) {
               transformer->name = utility::stripQuotations(currBlock["Name"]);
               std::string nomPower = currBlock["NominalPower"].substr(2,currBlock["NominalPower"].find_first_of(",")-2);
               transformer->capacity = utility::string2double(nomPower);
               std::string vOut = currBlock["Winding2"];
               std::string::size_type p1 = vOut.find_first_of("[")+1;
               std::string::size_type p2 = vOut.find_first_of(" ", p1+1);
               vOut = vOut.substr(p1, p2-p1);
               transformer->voltageOut = utility::string2double(vOut) / std::sqrt(3.0);
               std::cout << " - Found transformer with capacity " 
                         << std::setprecision(0) << transformer->capacity/1000 << " kVA" 
                         << " and output voltage " << transformer->voltageOut << std::endl;
               
               std::cout << " - Parsing the rest of the matlab model ... \n";
           }
           //else if(isRoot)
           //    rootBlocks.push_back(currBlock);
       }
       
       // If we have found the start of a model line
       else if(tokens.size() == 2 && tokens.at(0) == "Line" && tokens.at(1) == "{") {
           int innerDepthCounter = 1;
           thisModelLine.clear();
           
           // Looping through all parts of this line including branches
           while(innerDepthCounter > 0) {
                getline(modelFile,line);
                utility::tokenize(line, tokens, "\t ");
                
                // Keep track of depth
                if(tokens.size() == 2 && tokens.at(1) == "{")
                    innerDepthCounter++;
                else if(tokens.size() == 1 && tokens.at(0) == "}")
                    innerDepthCounter--;
                
                // Store all block names and ports.  SRC or DST doesn't matter.
                else if(tokens.at(0) == "SrcBlock" || tokens.at(0) == "DstBlock") {
                    pos1 = line.find_first_not_of("\t ", 0);
                    pos2     = line.find_first_of("\t ", pos1);
                    pos1 = line.find_first_not_of("\t ", pos2+1);
                    value = line.substr(pos1, line.length()-pos1);
                    thisBP.block = value;
                    getline(modelFile,line);
                    utility::tokenize(line, tokens, "\t ");
                    thisBP.port = tokens.at(1);
                    thisModelLine.push_back(thisBP);
                }
           }
           
           modelLines.push_back(thisModelLine);
       }
   }
}


void MatlabInterface::createRoot(FeederPole* &root,
                                 std::map<std::string,Household*> &households,
                                 std::map<std::string,FeederLineSegment*> &lineSegments,
                                 std::vector< std::vector<blockPort> > modelLines) {
    
    root = new FeederPole();
    root->name = "Root";

    // iterate through all model lines
    for(std::vector< std::vector<blockPort> >::iterator it_outer=modelLines.begin(); it_outer!=modelLines.end(); ++it_outer) {
        bool isRoot = false;
        Phase thisPhase;
        
        // within each model line, iterate once through all connected components, checking if this line connects to root
        for(std::vector<blockPort>::iterator it_inner=it_outer->begin(); it_inner!=it_outer->end(); ++it_inner) {
            if(it_inner->block == "\"PhaseA\"") {
                isRoot = true;
                thisPhase = A;
            }
            if(it_inner->block == "\"PhaseB\"") {
                isRoot = true;
                thisPhase = B;
            }
            if(it_inner->block == "\"PhaseC\"") {
                isRoot = true;
                thisPhase = C;
            }

        }
        
        // if this model line is connected to root, add any connected houses and lines to pole (ensuring no duplicates)
        if(isRoot) {
            for(std::vector<blockPort>::iterator it_inner=it_outer->begin(); it_inner!=it_outer->end(); ++it_inner){
                std::string thisBlockName = utility::stripQuotations(it_inner->block);
                if(households.find(thisBlockName) != households.end()) {
                    households[thisBlockName]->phase = thisPhase;
                    households[thisBlockName]->hasParent = true;
                    households[thisBlockName]->parentPoleName = root->name;
                    // Only add if it's not already there
                    if(std::find(root->households.begin(), root->households.end(), households[thisBlockName]) == root->households.end())
                        root->households.push_back(households[thisBlockName]);
                }
                else if(lineSegments.find(thisBlockName) != lineSegments.end()) {
                    lineSegments[thisBlockName]->parentPole = root;
                    lineSegments[thisBlockName]->parentPoleName = root->name;
                    // Only add if it's not already there
                    if(std::find(root->childLineSegments.begin(), root->childLineSegments.end(), lineSegments[thisBlockName]) == root->childLineSegments.end())
                        root->childLineSegments.push_back(lineSegments[thisBlockName]);
                }
            }
        }
    }
}

int find_portnum(std::string portname) {
    // num is last character of name
    return utility::string2int(portname.substr(portname.length()-1, 1));
}

void MatlabInterface::buildTree(FeederLineSegment* &root,
                                std::map<std::string,FeederPole*> &poles,
                                std::map<std::string,FeederLineSegment*> &lineSegments,
                                std::map<std::string,Household*> &households,
                                std::vector< std::vector<blockPort> > modelLines) {
    
    bool foundConnections = false;
    Phase thisPhase;
    
    FeederPole* newPole = new FeederPole();
    newPole->name = "Pole_" + root->name;
    newPole->parentLineSegment = root;
    
    // iterate through all model lines
    for(std::vector< std::vector<blockPort> >::iterator it_outer=modelLines.begin(); it_outer!=modelLines.end(); ++it_outer) {
        bool isConnected = false;
 
        // within each model line, iterate once through all connected components, 
        // checking if this line connects to root (note: ignore neutral line)
        for(std::vector<blockPort>::iterator it_inner=it_outer->begin(); it_inner!=it_outer->end(); ++it_inner)
            if(utility::stripQuotations(it_inner->block) == root->name && find_portnum(it_inner->port) != 4) {
                isConnected = true;
                foundConnections = true;
                thisPhase = Phase(find_portnum(it_inner->port) - 1);
                break;
            }

        // if this model line is connected to root, add any connected houses and lines to pole (ensuring no duplicates)
        if(isConnected) {
            for(std::vector<blockPort>::iterator it_inner=it_outer->begin(); it_inner!=it_outer->end(); ++it_inner){
                std::string thisBlockName = utility::stripQuotations(it_inner->block);
                
                // if there is a house on this line
                if(households.find(thisBlockName) != households.end()) {
                    // make sure this house doesn't already have a parent
                    if(!households[thisBlockName]->hasParent) {
                        households[thisBlockName]->phase = thisPhase;
                        households[thisBlockName]->hasParent = true;
                        households[thisBlockName]->parentPoleName = newPole->name;
                        // if not already added (not sure if it ever would be), add to list for this pole
                        if(std::find(newPole->households.begin(), newPole->households.end(), households[thisBlockName]) == newPole->households.end())
                            newPole->households.push_back(households[thisBlockName]);
                    }
                }
                
                // if there is a powerLine on this line
                else if(lineSegments.find(thisBlockName) != lineSegments.end()) {
                    // make sure it doesn't already have a parent 
                    // (would need to change this for non-radial networks)
                    if(lineSegments[thisBlockName]->parentPoleName.empty()) {
                        lineSegments[thisBlockName]->parentPole = newPole;
                        lineSegments[thisBlockName]->parentPoleName = newPole->name;
                        // if not already added (not sure why it would be), add to list for this pole
                        if(std::find(newPole->childLineSegments.begin(), newPole->childLineSegments.end(), lineSegments[thisBlockName]) == newPole->childLineSegments.end())
                            newPole->childLineSegments.push_back(lineSegments[thisBlockName]);
                    }
                }
            }
        }
    }
    
    if(foundConnections) {
        poles[newPole->name] = newPole;
        root->childPole = newPole;
        root->childPoleName = newPole->name;
        //std::cout << "Adding Pole " << newPole->name
        //     << " (parent " << newPole->parentLineSegment->name << ")"
        //     << " with " << (newPole->households.empty()?0:newPole->households.size()) << " houses (";
        //for(int i=0; i<newPole->households.size(); i++)
        //    std::cout << newPole->households.at(i)->name << ",";
        //std::cout << ") and " << (newPole->childLineSegments.empty()?0:newPole->childLineSegments.size()) << " child lines (";
        //for(int i=0; i<newPole->childLineSegments.size(); i++)
        //    std::cout << newPole->childLineSegments.at(i)->name << ",";
        //std::cout << ")" << std::endl;
        for(int i=0; i<newPole->childLineSegments.size(); i++)
            buildTree(newPole->childLineSegments.at(i), poles, lineSegments, households, modelLines);
    }
}



void MatlabInterface::extractModel(FeederPole* &root, 
                                   DistributionTransformer* &transformer,
                                   std::map<std::string,FeederPole*> &poles, 
                                   std::map<std::string,FeederLineSegment*> &lineSegments, 
                                   std::map<std::string,Household*> &households) {
    
    // "modelLines" are lines within the matlab model file
    // do not confuse with "lineSegments" which are piecewise segments of feeder line
    std::vector< std::vector<blockPort>  > modelLines;
    parseModelFile(transformer, lineSegments, households, modelLines);
    std::cout << " - Model contains " << households.size() << " houses, " 
                         << lineSegments.size() << " feeder line segments, and " 
                         << modelLines.size() << " model lines." << std::endl;
    //std::cout << " - Houses are: ";
    //for(std::map<std::string,Household*>::iterator it = households.begin(); it!=households.end(); ++it)
    //    std::cout << it->first << ",";
    //std::cout<<std::endl;
    
    // For later reference (when writing output to file e.g.), save household
    // names in workspace
    ss.str("");
    ss << "householdNames = {'" << households.begin()->second->name << "'";
    std::map<std::string,Household*>::iterator it = households.begin();
    it++;
    for( ; it != households.end(); ++it)
        ss << std::endl << "'" << it->second->name << "'";
    ss << "};" << std::endl;
    engEvalString(eng, ss.str().c_str());
    
    // Create root       
    createRoot(root, households, lineSegments, modelLines);
    poles[root->name] = root;
    //std::cout << " - Root has " << root->childLineSegments.size() << " child lines and " << root->households.size() << " houses." << std::endl;
 
    // Recursively build tree
    std::cout << " - Building network tree ..." << std::endl;
    for(int i=0; i<root->childLineSegments.size(); i++)
        buildTree(root->childLineSegments.at(i), poles, lineSegments, households, modelLines);
    
    // Double check all ok
    //for(std::map<std::string,FeederPole*>::iterator it=poles.begin(); it!=poles.end(); ++it) 
    //    std::cout << "Pole " << it->second->name << " has " << it->second->households.size() << " houses" << std::endl;
    
    // For later reference (when writing output to file e.g.), save backbone
    // names in workspace
    ss.str("");
    ss << "backboneNames = {'" << lineSegments.begin()->second->name << "'";
    std::map<std::string,FeederLineSegment*>::iterator it2 = lineSegments.begin();
    it2++;
    for( ; it2 != lineSegments.end(); ++it2)
        ss << std::endl << "'" << it2->second->name << "'";
    ss << "};" << std::endl;
    engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::runSim() {
    ss.str("");
    ss << "sim" << "('" << modelDir << modelName << "');";
    engEvalString(eng, ss.str().c_str()); 
}

double* MatlabInterface::getVar(std::string var) {
    result = engGetVariable(eng, var.c_str());
    if(result == NULL)
        return NULL;
    
    return ((double *)mxGetData(result));
}

void MatlabInterface::setVar(std::string component, double value, std::string var) {
    ss.str("");
    ss << "set_param('" << component << "','" << var << "','" << value << "');";
    engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::setVar(std::string component, std::string value, std::string var) {
    ss.str("");
    ss << "set_param('" << component << "','" << var << "','" << value << "');";
    engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::setTxCapacity(std::string component, double val) {
    // Matlab does a strange thing where it returns a name with a '\n' in it,
    // but then doesn't recognise a call for the component having that name
    // In short, replace \n with a space if there is one
    for(int i=0; i<component.length()-1; ++i) {
        if(component[i] == '\\' && component[i+1] == 'n') {
            component = component.substr(0,i) + " " + component.substr(i+2, component.length()-i+2);
            break;
        }
    }
    
    // Next, matlab needs a two-component matrix to specify transformer's
    // nominal power:  [capacity(VA) frequency(Hz)]
    std::stringstream ss;
    ss << "[" << val << " " << config->getString("frequency") << "]";
    setVar(modelNameRoot + "/" + component, ss.str(), "NominalPower");
}

int MatlabInterface::getNumHouses() {
    return 0;
}

std::vector <std::string> MatlabInterface::getHouseNames() {
    mwSize bufferLength;
    mxArray *componentName;
    char* currHouseName;
    std::string currHouseNameStr;
    int numHouses;
    std::vector <std::string> houseNames;

    ss.str("");
    ss << "A = find_system('" << modelName << "', 'FollowLinks', 'on', 'LookUnderMasks', 'on', 'name', 'House');"; //'regexp', 'on',
    //std::cout << ss.str() << std::endl;
    //std::cout.flush();
    engEvalString(eng, ss.str().c_str());
    // Should now have all house names stored in A
    
    ss.str("");
    ss << "[num x] = size(A);";
    engEvalString(eng, ss.str().c_str());
    result = engGetVariable(eng, "num");
    numHouses = (int)(*((double *)mxGetData(result)));
    //std::cout<<"FOUND " << numHouses << " HOUSES" << std::endl;
    
    for(int i=1; i<=numHouses; i++) {
        ss.str("");
        ss << "curr = A(" << i << ");";
        engEvalString(eng, ss.str().c_str());
        result = engGetVariable(eng, "curr");
        componentName = mxGetCell(result, 0);
        
        bufferLength = mxGetNumberOfElements(componentName) + 1;
        currHouseName = (char *)mxCalloc(bufferLength, sizeof(char));
        mxGetString(componentName, currHouseName, bufferLength);
                
        currHouseNameStr = std::string(currHouseName).c_str();
        houseNames.push_back(currHouseNameStr);
    }
    return houseNames;
}

void MatlabInterface::addVehicle(Vehicle vehicle) {
     ss.str("");
     int lastindex = vehicle.componentName.find_last_of("/");
     std::string subsysRoot = vehicle.componentName.substr(0,lastindex);
     std::string houseComponentName = vehicle.componentName.substr(0,lastindex).append("/House");
     
     ss << "component = find_system('" << houseComponentName << "');"
        << "set_param('" << subsysRoot << "', 'LinkStatus', 'inactive');"
        << "EV = add_block('" << houseComponentName << "','" << vehicle.componentName << "');"
        << "HH = component{1};"
        << "pos = get_param(HH,'Position') + 60;"
        << "set_param(EV, 'Position', pos);"
        << "set_param(EV, 'NamePlacement', 'normal');"
        << "portsEV = get_param(EV, 'PortHandles');"
        << "portsHH = get_param(HH, 'PortHandles');"
        << "add_line('" << subsysRoot << "', portsEV.LConn, portsHH.LConn);"
        << "add_line('" << subsysRoot << "', portsEV.RConn, portsHH.RConn);";
     engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::setDemand(std::string component, double active, double inductive, double capacitive) {
    setVar(component, active+0.001, "ActivePower");  // At least one value must be non zero
    setVar(component, inductive, "InductivePower");
    setVar(component, capacitive, "CapacitivePower");
}

void MatlabInterface::setDemand(std::string filename) {
     ss.str("");
     ss << "setloads('" << filename << "');";
     engEvalString(eng, ss.str().c_str());
}

void MatlabInterface::printModel(std::string targetDir) {
     std::cout << " - Printing grid model to file ... ";
     std::cout.flush();    
     ss.str("");
     ss << "saveas(systemHandle, '" << targetDir << "matlabModel.pdf','pdf');";
     engEvalString(eng, ss.str().c_str());
     std::cout << "OK" << std::endl;
}

void MatlabInterface::getOutputs(std::string logDir,
                                NetworkData &networkData, 
                                std::map<std::string,Household*> &households, 
                                std::map<std::string,FeederLineSegment*> &lineSegments,
                                std::map<std::string,FeederPole*> &poles) {
    ss.str("");
    ss << "logDir = '" << logDir << "';" << std::endl;
    ss << "writeOutputToFile;";
    engEvalString(eng, ss.str().c_str());
    
    std::ifstream infile(std::string(logDir + "temp_output.txt").c_str());
    std::string line;
    
    // Read in voltages
    for(int i=0; i<12; i++) {
        getline(infile, line);
        networkData.phaseV[i] = utility::string2double(line);
        //std::cout << networkData.phaseV[i] << std::endl;
    }

    // Read in currents
    for(int i=0; i<12; i++) {
        getline(infile, line);
        networkData.phaseI[i] = utility::string2double(line);
        //std::cout << networkData.phaseI[i] << std::endl;
    }

    // Read in eolV
    for(int i=0; i<12; i++) {
        getline(infile, line);
        networkData.eolV[i] = utility::string2double(line);
        //std::cout << networkData.eolV[i] << std::endl;
    }

    // Read in household V
    for(std::map<std::string, Household*>::iterator it = households.begin(); it!=households.end(); ++it) {
        getline(infile, line);
        it->second->V_RMS = utility::string2double(line);
        getline(infile, line);
        it->second->V_Mag = utility::string2double(line);
        getline(infile, line);
        it->second->V_Pha = utility::string2double(line);
        //std::cout << it->second->V_RMS << std::endl;
    }

    // Read in backbone V (unbalance)
    Phasor V_ab, V_bc, V_ca;
    Phasor v0, v1, v2;
    double a, p, unbalance;
    Household* currHouse;
    FeederPole currPole;

    for(std::map<std::string,FeederLineSegment*>::iterator it = lineSegments.begin(); it!=lineSegments.end(); ++it) {
        getline(infile, line);
        a = utility::string2double(line);
        getline(infile, line);
        p = utility::string2double(line);
        V_ab.set(a,p);
        
        getline(infile, line);
        a = utility::string2double(line);
        getline(infile, line);
        p = utility::string2double(line);
        V_bc.set(a,p);
        
        getline(infile, line);
        a = utility::string2double(line);
        getline(infile, line);
        p = utility::string2double(line);
        V_ca.set(a,p);
        
        unbalance = power::calculatePhaseUnbalance(V_ab, V_bc, V_ca);
        
        //  Calculate symmetrical components.
        //  Could really turn above line (unbalance) and below line (symmetrical 
        //  components) into single function but may be useful to keep them separate.
        power::symmetricalComponents(V_ab, V_bc, V_ca, v0, v1, v2);
        
        it->second->voltageUnbalance = unbalance;

        // Set individual houses' unbalance
        for(std::map<std::string,Household*>::iterator it2 = households.begin(); it2!=households.end(); ++it2) {
            currHouse = it2->second;
            
            // No unbalance at root (transformer as a voltage source assumption)
            if(currHouse->hasParent && currHouse->parentPoleName != "Root") {
                currPole = *(poles[currHouse->parentPoleName]);
                if(currPole.parentLineSegment->name == it->first) {
                    currHouse->V_unbalance = unbalance;
                    currHouse->V_0 = v0.getAmplitude();
                    currHouse->V_1 = v1.getAmplitude();
                    currHouse->V_2 = v2.getAmplitude();   
                    //std::cout << currHouse->name << " has phase " << currHouse->phase 
                    //        << " v0 " << currHouse->V_0 << " v1 " << currHouse->V_1 
                    //        << " v2 " << currHouse->V_2 << std::endl;
                }
            }
        }
    }
  
    // temp storage of a copy for debug
    //boost::filesystem::path pathFrom(std::string(logDir + "temp_output.txt"));
    //boost::filesystem::path pathTo(std::string(logDir + "../temp_output.txt"));
    //if (boost::filesystem::exists(pathTo))
    //    boost::filesystem::remove (pathTo);
    //boost::filesystem::copy_file(pathFrom, pathTo);
}



void MatlabInterface::generateReport(std::string dir, int month, bool isWeekday, int simInterval) {
    // Generate household demand plot
    ss.str("");
    ss << "plotHHDemand('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate EV behaviour plot
    ss.str("");
    ss << "plotEVbehaviour('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate EV demand plot
    ss.str("");
    ss << "plotEVDemand('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate Total demand plot
    ss.str("");
    ss << "plotTotalDemand('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate spot price plot
    ss.str("");
    ss << "plotSpotPrice('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase current and voltage plot RMS
    ss.str("");
    ss << "plotPhaseVI_RMS('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate individual household voltage plot
    ss.str("");
    ss << "plotHouseholdV('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase voltage plot waveform
    ss.str("");
    ss << "plotPhaseV_Wave('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase current plot waveform
    ss.str("");
    ss << "plotPhaseI_Wave('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase power factors plot
    ss.str("");
    ss << "plotPowerFactor('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate battery SOC plot
    ss.str("");
    ss << "plotBatterySOC('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase unbalance plot by pole (true unbalance
    ss.str("");
    ss << "plotPhaseUnbalanceTrue('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // Generate phase unbalance plot by percent deviation from average phase load
    ss.str("");
    ss << "plotPhaseUnbalanceDeviation('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());

    // If this was a run involving optimisation, also plot optimisation fval and exit flags
    std::string optDir = dir+"optdata/";
    if(boost::filesystem::exists(optDir)) {
        ss.str("");
        ss << "plotOptimisationOutput('" << optDir << "', " << simInterval << ");";
        engEvalString(eng, ss.str().c_str());
    }
    
    // Generate price of charging comparison
    ss.str("");
    ss << "plotCostOfCharging('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());
        
    // Generate charge rate standard deviation plot
    ss.str("");
    ss << "plotChargeRateStats('" << dir << "', " << simInterval << ");";
    engEvalString(eng, ss.str().c_str());
        
}


void MatlabInterface::runOptimisationLinear(std::string optDir, std::string optAlg, 
                                      int numDecVars, int numConstraints, 
                                      double &fval, double &exitflag) {
    boost::posix_time::ptime timer;
    
    std::cout << " - Now running optimisation in matlab ..." << std::endl;
    
    utility::startTimer(timer);
    std::cout << "   - reading in matrix A ...";
    // Matrix A is set of coordinates and values
    ss.str("");
    ss << "A = zeros(" << numConstraints << "," << numDecVars << ");" << std::endl
       << "A_in = dlmread(['" << optDir << "' 'A.txt']);" << std::endl
       << "[rows cols] = size(A_in);" << std::endl
       << "for i=1:rows" << std::endl
       << "      A(A_in(i,1)+1, A_in(i,2)+1) = A_in(i,3);" << std::endl
       << "end" << std::endl;
    engEvalString(eng, ss.str().c_str());
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;

    std::cout << "   - reading in other matrices ...";
    ss.str("");
    ss << "c = dlmread(['" << optDir << "' 'c.txt']);" << std::endl
       << "b = dlmread(['" << optDir << "' 'b.txt']);" << std::endl
       << "lb = dlmread(['" << optDir << "' 'lb.txt']);" << std::endl
       << "ub = dlmread(['" << optDir << "' 'ub.txt']);" << std::endl;
    engEvalString(eng, ss.str().c_str());
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;
    
    ss.str("");
    std::cout << "   - running optimisation ...";
    if(optAlg == "simplex")
        ss << "options = optimset('LargeScale', 'off', 'Simplex', 'on');" << std::endl
           << "[xsol,fval,exitflag]=linprog(c,A,b,[],[],lb,ub,[],options);";
    else
        ss << "[xsol,fval,exitflag]=linprog(c,A,b,[],[],lb,ub,[],[]);";
    engEvalString(eng, ss.str().c_str());
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;
    
    std::cout << "   - writing output to file ...";
    std::cout.flush();
    ss.str("");
    ss << "dlmwrite(['" << optDir << "' 'xsol.txt'], xsol);" << std::endl;
    engEvalString(eng, ss.str().c_str());

    // It is possible that the optimisation fails during preprocessing in which
    // case fval does not exist
    if(getVar("fval") == NULL) {
        fval = -1;
        exitflag = -1;    
    }
    else {
        fval = -1*(*getVar("fval"));
        exitflag = (*getVar("exitflag"));    
    }
    std::cout << " OK (took " << utility::updateTimer(timer) << ")" << std::endl;
}