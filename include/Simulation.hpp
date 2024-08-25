#ifndef DEF_SIMULATION_HPP
#define DEF_SIMULATION_HPP

#include <vector>
#include <iostream>
#include <random>
#include <list>
#include <cmath>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include "VariableEnv.hpp"
#include "Population.hpp"
#include "Environment.hpp"
#include "Functor.hpp"
#include "Vecteur.hpp"
#include "Display.hpp"


using namespace std;

//======================================================================
//                           Class Processus 
//======================================================================

class Simulation
{
    public :
        Environment_t  environment; 
        Vecteur<Vecteur<float>> countVector;
        int timeBeforeStationarity;                       

        //Constructor               
        Simulation(const Environment_t& env_init, int nIter, bool plot); //Constructor of the function 
};

//======================================================================
//                           Member functions
//======================================================================

Simulation::Simulation(const Environment_t& env_init, int nIter, bool plot)
{   
    //Initialization
    environment = env_init;
    timeBeforeStationarity = 0;     
    countVector.resize(environment.species.size());

    //count the populations
    Vecteur<float> counts = environment.countPopulations();
    for (int k=0; k<environment.species.size(); k++){countVector[k].push_back(counts[k]);}
    
    //Dimensions to display in the environment
    int dimension=0;

    //Generate pixel array
    if (plot==true){environment.display("merged", dimension);}

    for (int i=1; i<nIter; i++)
    {   
        //Keep in memory the old environment
        Environment_t oldEnvironment(environment);

        //environment=selection(diffusion(environmentalChange(environment, i, a, b)));
        environment=environment.migration().selection();

        //Count the populations
        //Vecteur<float> counts = environment.countPopulations();
        //for (int k=0; k<environment.species.size(); k++){countVector[k].push_back(counts[k]);}

        //Display settings
        if (plot==true){environment.display("merged", dimension, i);}

        //Check if the automaton is still avancing
        if (oldEnvironment == environment & timeBeforeStationarity==0){timeBeforeStationarity=i-1; break;}
    }
}

//======================================================================
//                          External functions
//======================================================================




#endif