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

//======================================================================
//Class Environment_t at time t definition
//======================================================================

template<typename T>
class Environment_t
{
    public :
        int n;                                                        //Size of the grid
        float unit;                                                   //Unit of the grid
        vector<VariableEnv<T>> conditions;                            //Environmental matrix
        vector<Specie<T>> species;                                    //List of species that live in the Environment_t
        vector<vector<Specie<T>>> repartition;                        //Species repartition matrix

        //Constructors
        Environment_t(){};                                            //Empty constructor
        Environment_t(float unit, vector<Specie<T>> sp, int m, float a, float b);       //Constructor of an environment matrix using functors for initial species repartition and environmental conditions 
};

//======================================================================
// Member functions
//======================================================================

//Constructor
template<typename T>
Environment_t<T>::Environment_t(float unitEnv, vector<Specie<T>> sp, int m, float a, float b)
{
    n = m;            //Size of the environmental matrix
    unit=unitEnv;     //Unit of the environment

    //Construction of the environmental matrix
    conditions.resize(n*n);
    envFunctor intialEnv; 
    intialEnv(conditions, n, unit, a);

    //Species
    species = sp;

    //Construction of the intial repartition
    repartition.resize(n*n);
    repFunctor initialRep;
    //vector<vector<Specie<T>>> rep(n*n, vector<Specie<T>> (1, sp[0]));
    initialRep(repartition, n, sp);
}

//======================================================================
//External functions/operators
//======================================================================

template<typename T>
ostream& operator <<(ostream & out, const Environment_t<T>& E)
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

