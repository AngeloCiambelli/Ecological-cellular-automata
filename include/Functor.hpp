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
    float coeff = (10-int(t)%10)*a;
    //if (i <= (n-1)/2){newEnv = Vecteur<float>({float(1)});} 
    VariableEnv<Vecteur<float>> newEnv(Vecteur<float>({(sin(float(i*unit)*M_PIf*a/2+t*b)*cos(float(j*unit))+1.f)}));  // add additional dimensions of the env if needed (nD) //{float((i*unit)/((n-1)*unit))}
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
        if (i%2==1 /* &j==(n-1)/2)*/) {rep[i*n+j].push_back(sp[0]);}
        else {rep[i*n+j].push_back(sp[1]);}
        //else if (i==3*(n-1)/4 /*& j==3*(n-1)/4*/){rep[i*n+j].push_back(sp[1]);}
      }
    }
    return(rep);
  }
};

// Functor to set the initial environment
class envFunctor
{
public:
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, int n, float unit, float a)
  {
    for (int i=0; i<n; i++){
      for (int j=0; j<n; j++)
      {
        //if (i <= (n-1)/2){env[i*n+j].parameters=Vecteur<float>({1});}
        //else {env[i*n+j].parameters=Vecteur<float>({0});}
        env[i*n+j].parameters=Vecteur<float>({sin(float(i*unit)*M_PIf*a)*cos(float(j*unit))});  // add additional dimensions of the env if needed (nD) {float((i*unit)/((n-1)*unit))}
      }
    }
    return(env);
  }
};

#endif