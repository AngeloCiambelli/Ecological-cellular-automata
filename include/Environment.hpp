#ifndef DEF_ENVIRONMENT_HPP
#define DEF_ENVIRONMENT_HPP

#include <vector>
#include <iostream>
#include <random>
#include <list>
#include <map>
#include <math.h>
#include <string>
#include "VariableEnv.hpp"
#include "Species.hpp"
#include "Functor.hpp"

using namespace std;

string makeName(string filename, map<string,float> parameters, vector<Specie> sp){
    
    //Add the parameters to the filename
    for (auto it=parameters.begin(); it!=parameters.end(); ++it){
        std::ostringstream value; value << std::fixed << std::setprecision(1) << it->second;
        filename += it->first + "=" + value.str() + "\n ";
    }

    for (int i = 0; i < sp.size(); i++) {
        std::ostringstream niche_stream; niche_stream << std::fixed << std::setprecision(1) << sp[i].niche.parameters[0];
        filename += sp[i].name + ":[{" + niche_stream.str() + "}," + to_string(sp[i].diffusion_speed) + "]";
        if (i<sp.size()-1){filename += "_";} else {filename+="\n ";}
    }
    
    return(filename);
}

//======================================================================
//Class Environment_t at time t definition
//======================================================================

class Environment_t
{
    public :
        int n;                                                    //Size of the lattice
        float unit;                                               //Unit of the lattice
        float envDilatation;                                      //Parameter of dilatation to set or change the environment
        float envDelay;                                           //Parameter of delay to set or change the environment
        float persistency;                                        //Persistency of the species (how much better do you have to be at a lattice point to replace it)
        string name;                                              //Name of the environnment with its characteristics
        float distMean;                                           //Distribution mean used if the environment have random generation
        float distVar;                                            //Distribution variance used if the environment have random generation

        vector<VariableEnv<Vecteur<float>>> conditions;           //Environmental matrix
        vector<Specie> species;                                   //List of species that live in the Environment_t
        vector<vector<Specie>> repartition;                       //Species repartition matrix
        vector<int> numberOfChanges;                              //Number of changes in the occupancy for every spot in the lattice
        
        //Constructors
        Environment_t(){};                                                                                    //Empty constructor
        Environment_t(vector<Specie> sp, map<string,float> parameters, string filename, string GenType);      //Constructor of an environment matrix using functors for initial species repartition and environmental conditions 
        
};

//======================================================================
// Member functions
//======================================================================

//float unitEnv, int m, float a, float b,
//Constructor
Environment_t::Environment_t(vector<Specie> sp, map<string,float> parameters, string filename, string genType) : numberOfChanges(int(parameters["n"]*parameters["n"]), 0)
{   
    //Extract the parameters
    n = parameters["n"];                  
    if (parameters.find("unit") != parameters.end()){envDilatation = parameters["unit"];} 
    if (parameters.find("envDilatation") != parameters.end()){envDilatation = parameters["envDilatation"];}        
    if (parameters.find("envDelay") != parameters.end()){envDelay = parameters["envDelay"];}
    if (parameters.find("distMean") != parameters.end()){distMean = parameters["distMean"];} 
    if (parameters.find("distVar") != parameters.end()){distVar = parameters["distVar"];}
    if (parameters.find("persistency") != parameters.end()){persistency = parameters["persistency"];}  

    //Construction of the environmental matrix
    conditions.resize(n*n);
    envFunctor intialEnv; 
    intialEnv(conditions, parameters, genType);

    //Species
    species = sp;

    //Construction of the intial repartition
    repartition.resize(n*n);
    repFunctor initialRep;
    initialRep(repartition, n, sp);

    //Add the parameters and species used in the name of the environment
    name = makeName(filename, parameters, sp);
}

//======================================================================
//External functions/operators
//======================================================================

ostream& operator <<(ostream & out, const Environment_t& E)
{   
    for(int i=0; i<E.n; i++)
        {
            out << "[";
            for (int j=0; j<E.n; j++)
            {
                if (E.repartition[i*E.n+j].size()!=0) {out << E.repartition[i*E.n+j][0].name << " ";}
                else {out << "  ";}
            }
            out << "]";
            for (int j=0; j<E.n; j++)
            {
                out << E.conditions[i*E.n+j].parameters << " ";
            }
            out << endl;
        }
    return out;
}

#endif

