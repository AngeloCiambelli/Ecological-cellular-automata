#ifndef DEF_FUNCTOR_HPP
#define DEF_FUNCTOR_HPP

#include <vector>
#include <iostream>
#include <random>
#include <list>
#include <map>
#include <cmath>
#include <math.h>
#include <string>
#include "VariableEnv.hpp"
#include "Species.hpp"
#include "Environment.hpp"
#include "Vecteur.hpp"

using namespace std;

//======================================================================
//                           Class envFunctor 
//======================================================================

// Functor used to modify the environment
class envChangeFunctor
{
public:
  VariableEnv<Vecteur<float>> operator()(float unit, float i, float j, float n, float t, float a, float b) const
  {
    //Vecteur<float> newEnv({float(0)});
    //float coeff = (10-int(t)%10)*a;
    //if (i <= (n-1)/2){newEnv = Vecteur<float>({float(1)});} 
    VariableEnv<Vecteur<float>> newEnv(Vecteur<float>({0.5f*sin(a*i*unit+b*t)+0.5f}));  // add additional dimensions of the env if needed (nD) //{float((i*unit)/((n-1)*unit))}
    return VariableEnv(newEnv);
  }
};

// Functor used to seed the original environment of species
class repFunctor
{
public:
  vector<vector<Specie>>& operator()(vector<vector<Specie>>& rep, int n, vector<Specie> sp)
  {
    for (int i=0; i<n; i++){
      for (int j=0; j<n; j++)
      {
        if (i==n-1) {rep[i*n+j].push_back(sp[0]);} //Alternated: i%2==1 
        else {rep[i*n+j].push_back(sp[1]);}
        //else if (i==3*(n-1)/4 /*& j==3*(n-1)/4*/){rep[i*n+j].push_back(sp[1]);}
      }
    }
    return(rep);
  }
};

// Functor to set the initial environment (determinist and probabilist generation)
class envFunctor
{
public:
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, map<string,float> parameters, string gen)
  {
    if (gen=="function"){
      for (int i=0; i<parameters["n"]; i++){
        for (int j=0; j<parameters["n"]; j++)
        {
          env[i*parameters["n"]+j].parameters=Vecteur<float>({sin(0.5f*float(parameters["envDilatation"]*i*parameters["unit"])+0.5f)});  // add additional dimensions of the env if needed (nD) {float((i*unit)/((n-1)*unit))}
        }
      }
    }
    
    else if(gen=="random"){
      for (int i=0; i<parameters["n"]; i++){
        for (int j=0; j<parameters["n"]; j++)
        {
          std::random_device rd{};
          std::mt19937 gen{rd()};
          normal_distribution<float> dist(parameters["distMean"],sqrt(parameters["distVar"]));
          env[i*parameters["n"]+j].parameters=Vecteur<float>({float(dist(gen))});  
        }
      }
    }

    return(env);
  }
};

#endif