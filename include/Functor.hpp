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
  VariableEnv<Vecteur<float>> operator()(float unit, double i, double j, double n, double t) const
  {
    VariableEnv<Vecteur<float>> newEnv(Vecteur<float>({float((i*unit)/((n-1)*unit)+0.001)}));  // add additional dimensions of the env if needed (nD)
    return VariableEnv(newEnv);
  }
};

// Functor used to seed the original environment of species
class repFunctor
{
public:
  vector<vector<Specie<Vecteur<float>>>>& operator()(vector<vector<Specie<Vecteur<float>>>>& rep, int n, vector<Specie<Vecteur<float>>> sp)
  {
    for (int i=0; i<n; i++){
      for (int j=0; j<n; j++)
      {
        if (i<n/2) {rep[i*n+j].push_back(sp[0]);}
        else {rep[i*n+j].push_back(sp[1]);}
      }
    }
    return(rep);
  }
};

// Functor to set the initial environment
class envFunctor
{
public:
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, int n, float unit)
  {
    for (int i=0; i<n; i++){
      for (int j=0; j<n; j++)
      {
        env[i*n+j].parameters=Vecteur<float>({float((i*unit)/((n-1)*unit)+0.001)});  // add additional dimensions of the env if needed (nD)
      }
    }
    return(env);
  }
};

#endif