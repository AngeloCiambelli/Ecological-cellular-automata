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

class Specie
{
public:
    string name;                    //Name of the specie
    VariableEnv<Vecteur<float>> niche;           //Optimal growing conditions
    int diffusion_speed;            //Number of case travelled per iterations during diffusion

    //Constructors
    Specie(VariableEnv<Vecteur<float>>& v, string specie, int speed);
    Specie(){};

    //Member functions

    //Operators
    bool operator==(const Specie &B) const {return(this->name==B.name);}
};

//======================================================================
// Member functions
//======================================================================

Specie::Specie(VariableEnv<Vecteur<float>>& v, string specie, int speed)
{
    niche=v;
    diffusion_speed=speed;
    name=specie;
}

//======================================================================
// External functions
//======================================================================

Specie mean(Specie A, Specie B)
{
    VariableEnv newNiche(A.niche.parameters+B.niche.parameters/float(2));
    Specie infant(newNiche,"("+A.name+B.name+")",min(A.diffusion_speed, B.diffusion_speed));
    return(infant);
}

ostream& operator <<(ostream & out, const Specie& sp)
{   
    out << sp.name << ":" << sp.niche ;
    return out;
}
#endif