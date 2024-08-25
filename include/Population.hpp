#ifndef DEF_POPULATION_HPP
#define DEF_POPULATION_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Vecteur.hpp"
#include "VariableEnv.hpp"

using namespace std;

//======================================================================
// Class Population definition
//======================================================================

class Population
{
public:
    string name;                            //Name of the Population
    VariableEnv<Vecteur<float>> niche;      //Optimal growing conditions
    int diffusion_speed;                    //Number of case travelled per iterations during diffusion
    Vecteur<Vecteur<float>> tolerance;      //Tolerance matrix of the Population

    //Constructors
    Population(VariableEnv<Vecteur<float>>& v, string Population, int speed, Vecteur<Vecteur<float>> tol);
    Population(){};

    //Member functions

    //Operators
    bool operator==(const Population &B) const {return(this->name==B.name);}
};

//======================================================================
// Member functions
//======================================================================

Population::Population(VariableEnv<Vecteur<float>>& v, string Population, int speed, Vecteur<Vecteur<float>> tol)
{
    niche=v;
    diffusion_speed=speed;
    name=Population;
    tolerance = tol;
}

//======================================================================
// External functions
//======================================================================

Population mean(Population A, Population B)
{
    VariableEnv newNiche(A.niche.parameters+B.niche.parameters/float(2));
    Population infant(newNiche,"("+A.name+B.name+")",min(A.diffusion_speed, B.diffusion_speed), A.tolerance);
    return(infant);
}

ostream& operator <<(ostream & out, const Population& sp)
{   
    out << sp.name << ":" << sp.niche ;
    return out;
}
#endif