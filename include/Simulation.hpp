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
#include "Species.hpp"
#include "Environment.hpp"
#include "Functor.hpp"
#include "Vecteur.hpp"
#include "Display.hpp"


using namespace std;

//======================================================================
//                           Class Processus 
//======================================================================

template<typename T>
class Simulation
{
    public :
        Environment_t<T>  environment;                                          
        Simulation(const Environment_t<T>& env_init, int nIter, float fecondity, bool plot, string filename, float a, float b); //Constructor of the function 
};

//======================================================================
//                           Member functions
//======================================================================

template<typename T>
Simulation<T>::Simulation(const Environment_t<T>& env_init, int nIter, float fecondity, bool plot, string filename, float a, float b)
{   
    //Initialization
    environment = env_init;
    //Dimensions to display in the environment
    int dimension=0;

    //Generate pixel array
    if (plot==true)
    {
        tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  repartitionPixels = repartitionToPixel(environment);
        tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  changePixels = changeToPixel(environment.numberOfChanges);
        tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  envPixels = envToPixel(environment.conditions, 0);

        imagePlot(repartitionPixels, 0, filename+"repartition.png", environment.n);
        imagePlot(changePixels, 0, filename + "numberChange.png", environment.n);
        imagePlot(envPixels, 0, filename + "environment dimension "+to_string(dimension)+".png", environment.n);
    }

    for (int i=1; i<nIter; i++)
    {
        environment=selection(diffusion(environmentalChange(environment, i, a, b)));
        
        //Display settings
        if (plot==true)
        {
            //Generate pixel array
            tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  repartitionPixels = repartitionToPixel(environment);
            tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  changePixels = changeToPixel(environment.numberOfChanges);
            tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  envPixels = envToPixel(environment.conditions, dimension);

            //Filenames
            string repFile = filename + "repartition \n t=" + to_string(i) + ".png";
            string changeFile = filename + "numberChange \n t=" + to_string(i) + ".png";
            string envFile = filename + "environment dimension " + to_string(dimension) +"\n t=" + to_string(i) + ".png";

            //Save image
            imagePlot(repartitionPixels, i, repFile, environment.n);
            imagePlot(changePixels, i, changeFile, environment.n);
            imagePlot(envPixels, i, envFile, environment.n);

            mergeImage(envFile, repFile, changeFile, "env_rep_change_t="+to_string(i)+".png");
        }
        else {cout << environment << endl;}
    }
}

//======================================================================
//                          External functions
//======================================================================

//Diffusion of the species on the grid (determinist)
template<typename T>
Environment_t<T> diffusion(Environment_t<T> env)
{
    //New environment
    Environment_t<T> newEnv(env);

    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            if (env.repartition[i*env.n+j].size()!=0)
            {
                for (int k=0; k<=(env.repartition[i*env.n+j][0]).diffusion_speed; k++){
                    for (int l=0; l<=(env.repartition[i*env.n+j][0]).diffusion_speed-k; l++){
                        if ((i+l < env.n) and (j+k < env.n))
                        {
                        newEnv.repartition[(i+l)*env.n+k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                        if ((i-l >= 0) and (j-k >= 0))
                        {
                        newEnv.repartition[(i-l)*env.n-k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                        if ((i+l < env.n) and (j-k >= 0))
                        {
                        newEnv.repartition[(i+l)*env.n-k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                        if ((i-l >= 0) and (j+k < env.n))
                        {
                        newEnv.repartition[(i-l)*env.n+k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                    }
                }
            }
        }
    }
    
    return newEnv;
}

//Selection of the best adapted specie in each node of the grid (determinist)
template<typename T>
Environment_t<T> selection(Environment_t<T> env)
{
    //New environment
    Environment_t<T> newEnv(env);

    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            float score(1000000);
            int ind(0);
            
            for (int k=0; k<env.repartition[i*env.n+j].size(); k++)
            {
                if ((env.repartition[i*env.n+j][k].niche.parameters - env.conditions[i*env.n+j].parameters).norm() / ((env.conditions[i*env.n+j].parameters).norm()+float(0.0001)) < score)
                {
                    ind = k;
                    score = (env.repartition[i*env.n+j][k].niche.parameters - env.conditions[i*env.n+j].parameters).norm() / ((env.conditions[i*env.n+j].parameters).norm()+float(0.0001));
                }
            }
            
            if (newEnv.repartition[i*env.n+j].size()!=0)
            {
                if (ind!=0)
                {
                    newEnv.numberOfChanges[i*env.n+j]+=1;
                }

                Vecteur<Specie<T>> bestSp({env.repartition[i*env.n+j][ind]});
                newEnv.repartition[i*env.n+j]=bestSp;
            }
        }
    }
    return newEnv;
}

//Change in the environment according to the functor : f_t(conditions)
template<typename T>
Environment_t<T> environmentalChange(Environment_t<T> env, float t, float a, float b)
{   
    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            envChangeFunctor f;
            env.conditions[i*env.n+j] = f(env.unit, i, j, env.n, t, a, b);
        }
    }
    return env;
} 

//Aleatory reproduction of species in each node of the grid chosing two parent species
template<typename T>
Environment_t<T> reproduction(Environment_t<T> env, float fecondity)
{   
    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            random_device rd;
            mt19937 gen(rd());
            bernoulli_distribution infant(fecondity);
            if (infant(gen)==true)
            {
                uniform_int_distribution<int> parentDistribution{  0, (env.repartition[i*env.n+j]).size()-1};
                Specie<T> parent1(env.repartition[i*env.n+j][parentDistribution(gen)]);
                Specie<T> parent2(env.repartition[i*env.n+j][parentDistribution(gen)]);

                //Ceck if specie already exist in the species list, if not add it
                if ((std::find(env.species.begin(), env.species.end(), mean(parent1,parent2)) != env.species.end())==false)
                {
                    env.species.push_back(mean(parent1,parent2));
                }
                env.repartition[i*env.n+j].push_back(mean(parent1,parent2));
            }  
        }
    }
    return env;
}

#endif