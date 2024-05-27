#ifndef DEF_SPECIES_HPP
#define DEF_SPECIES_HPP

#include <iostream>
#include <vector>
#include <string>
#include "VariableEnv.hpp"
#include "Environment.hpp"

using namespace std;

//======================================================================
// Class Specie definition
//======================================================================

template<typename T>
class Specie
{
public:
    string name;                    //Name of the specie
    VariableEnv<T> niche;           //Optimal growing conditions
    int diffusion_speed;            //Number of case travelled per iterations during diffusion

    //Constructors
    Specie(VariableEnv<T>& v, string specie, int speed);
    Specie(){};

    //Member functions
    bool operator==(const Specie<T> &B);
};

//======================================================================
// Member functions
//======================================================================

template<typename T>
Specie<T>::Specie(VariableEnv<T>& v, string specie, int speed)
{
    niche=v;
    diffusion_speed=speed;
    name=specie;
}

template<typename T>
bool Specie<T>::operator==(const Specie<T> &B)
{
    return((*this).name==B.name);
}

//======================================================================
// External functions
//======================================================================

template<typename T>
Specie<T> mean(Specie<T> A, Specie<T> B)
{
    VariableEnv<T> newNiche(A.niche.parameters+B.niche.parameters/float(2));
    Specie<T> infant(newNiche,"("+A.name+B.name+")",min(A.diffusion_speed, B.diffusion_speed));
    return(infant);
}

template<typename T>
ostream& operator <<(ostream & out, const Specie<T>& sp)
{   
    out << sp.name << ":" << sp.niche ;
    return out;
}
#endif