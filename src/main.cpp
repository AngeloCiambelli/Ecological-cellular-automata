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
    parameters["n"] = 200;                //Size of the lattice
    //parameters["unit"] = 0.1;             //Unit of the square (to simplify function use)
    //parameters["envDilatation"] = 5;      //Oscillation parameter
    //parameters["envDelay"] = 0;           //Speed of the travelling wave
    parameters["persistency"] = 0.5;      //Persistency of the species
    parameters["distMean"] = 0.5;         //Mean of species niche, mean of the distribution used for environment generation
    parameters["distVar"] = 0.5;          //Variance of the distribution used for environment generation

    int nIter=300;              //Number of iteration in the simulation
    bool plot = false;           //If you want to plot the results

    //Define the generic filename
    string filename="heterogenous=NormalDist \n constant \n ";

    //Creation of the species' niches
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({1}));
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({0}));

    //Creation of the species
    Specie A(niche_A,"A", 2);
    Specie B(niche_B,"B", 1);

    //Creation of the species list
    vector<Specie> spVector({A, B});

    //Creation of the environment
    //Environment_t E(spVector, parameters, filename, "random");

    //Running simulation
    //Simulation automate(E, nIter, plot, filename);

    
    vector<float> means({0,0.25,0.5,0.75,1});
    vector<float> variances({0.1,0.25,0.5,0.75,1});
    vector<float> persistencies({0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1});

   int padding = 50;

    for (int k=0; k<persistencies.size(); k++){
        sf::RenderTexture envRender;
        envRender.create(means.size()*parameters["n"]+(means.size()+1)*padding, variances.size()*parameters["n"]+(variances.size()+1)*padding);
        envRender.clear(sf::Color::White);

        for (int i=0; i<means.size(); i++){
            for (int j=0; j<variances.size(); j++){ 
                
                //Creation of the environment
                map<string, float> param;
                param["n"] = parameters["n"];
                param["distMean"] =  means[i];
                param["distVar"] = variances[j];
                param["persistency"] = persistencies[k];

                //Creation of the environment
                Environment_t E(spVector, param, filename, "random");

                //run simulation
                Simulation automate(E, nIter, plot, filename);

                tuple<sf::Uint8*, string, string, sf::Color, sf::Color> rep = repartitionToPixel(automate.environment);
                gridPlot(rep, envRender, automate.environment.n, i, j, padding, means[i], variances[j], "Mean", "Var");
            }
            cout << "line " << i << " finished \n";
        }
        //Display the result and save to a file
    envRender.display();
    sf::Image finalImage = envRender.getTexture().copyToImage();
    finalImage.saveToFile("/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/output/images/ " + filename + " gridMean&Variance \n persistency=" + to_string(persistencies[k]) + ".png");
    }

    cout << endl << "Finished" << endl;
}
