#include <iostream>
#include <random>
#include <iomanip>
#include "Vecteur.hpp"
#include "VariableEnv.hpp"
#include "Environment.hpp"
#include "Simulation.hpp"
#include "Functor.hpp"

int main(int argc, char *argv[])
{
    int latticeSize = 201;      //Size of the network lattice
    int nIter=50;              //Number of iteration in the simulation 
    float unit=0.1;             //Unit of the square (to simplify function use)
    float a = 4;               //Oscillation parameter
    float b = 0.1;              //Vitesse de deplacement de l'onde

    //Define the generic filename
    std::ostringstream unit_stream; unit_stream << std::fixed << std::setprecision(1) << unit; //To only print one significative number after the comma
    std::ostringstream a_stream; a_stream << std::fixed << std::setprecision(1) << a;
    std::ostringstream b_stream; b_stream << std::fixed << std::setprecision(1) << b;
    string filename="heterogenous=sin(i*unit)*cos(j*unit) \n variable=(10-t%10)*a*(sin(i*unit)*cos(j*unit))+(10-t%10)*a\n n=" + to_string(latticeSize) 
                        + "\n unit=" + unit_stream.str() + ", a=" + a_stream.str() + ", b=" + b_stream.str() + "\n ";

    //Creation of the species' niches
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({1}));
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({0}));

    //Creation of the species
    Specie<Vecteur<float>> A(niche_A,"A", 1);
    Specie<Vecteur<float>> B(niche_B,"B", 1);

    //Creation of the species list
    vector<Specie<Vecteur<float>>> sp_vect({A, B});

    //Creation of the environment
    Environment_t<Vecteur<float>> E(unit, sp_vect,  latticeSize, a, b);
    cout << E << endl << endl;

    //Running simulation
    Simulation<Vecteur<float>> automate(E, nIter, 0, true, filename, a, b);
    cout << endl << "Finished" << endl;
}
