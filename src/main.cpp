#include <iostream>
#include <random>
#include "Vecteur.hpp"
#include "VariableEnv.hpp"
#include "Environment.hpp"
#include "Simulation.hpp"
#include "Functor.hpp"

int main(int argc, char *argv[])
{
    int latticeSize = 101;      //Size of the network lattice
    int nIter=100;              //Number of iteration in the simulation 
    float unit=0.1;            //Unit of the square (to simplify function use)

    //Creation of the species' niches
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({1}));
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({0}));

    //Creation of the species
    Specie<Vecteur<float>> A(niche_A,"A", 1);
    Specie<Vecteur<float>> B(niche_B,"B", 1);

    //Creation of the species list
    vector<Specie<Vecteur<float>>> sp_vect({A, B});

    //Creation of the environment
    Environment_t<Vecteur<float>> E(unit, sp_vect,  latticeSize);
    cout << E << endl << endl;

    //Running simulation
    Simulation<Vecteur<float>> automate(E, nIter, 0, true);
    cout << endl << "Finished";
}
