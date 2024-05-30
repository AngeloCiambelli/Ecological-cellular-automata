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
    map<string, float> parameters;
    parameters["n"] = 201;                //Size of the lattice
    //parameters["unit"] = 0.1;             //Unit of the square (to simplify function use)
    //parameters["envDilatation"] = 0;      //Oscillation parameter
    //parameters["envDelay"] = 0;           //Speed of the travelling wave
    parameters["persistency"] = 0;      //Persistency of the species
    parameters["distMean"] = 0.5;         //Mean of species niche, mean of the distribution used for environment generation
    parameters["distVar"] = 0.5;          //Variance of the distribution used for environment generation

    int nIter=200;              //Number of iteration in the simulation
    bool plot = true;           //If you want to plot the results

    //Define the generic filename
    string filename="heterogenous=N(0.5,0.5) \n constant \n ";

    //Creation of the species' niches
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({1}));
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({0}));

    //Creation of the species
    Specie A(niche_A,"A", 1);
    Specie B(niche_B,"B", 1);

    //Creation of the species list
    vector<Specie> spVector({A, B});

    //Creation of the environment
    Environment_t E(spVector, parameters, filename, "random");

    //Running simulation
    Simulation automate(E, nIter, plot, filename);
    cout << endl << "Finished" << endl;
}
