#include <iostream>
#include <random>
#include <iomanip>
#include <cmath>
#include "Vecteur.hpp"
#include "VariableEnv.hpp"
#include "Environment.hpp"
#include "Simulation.hpp"
#include "Functor.hpp"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

int main(int argc, char *argv[])
{
    //Load environmental image
    sf::Image depthImage;
    sf::Image vegetationImage;
    depthImage.loadFromFile("include/roscoff_rocky_beach.png"); //Carefull with image size...
    vegetationImage.loadFromFile("include/vegetation_cover.png");
    sf::Vector2u size = depthImage.getSize();
    sf::Vector2u sizeVege = vegetationImage.getSize();
    if(size.x != sizeVege.x | size.y != sizeVege.y){cout << "image size don't match \n"; exit(1);} //Check before if the two images have the same size

    map<string, float> parameters;
    parameters["n"] = size.x;               //Number of columns of the lattice
    parameters["m"] = size.y;               //Number of rows of the lattice
    //parameters["unit"] = 0.1;             //Unit of the square (to simplify function use)
    //parameters["envDilatation"] = 5;      //Oscillation parameter
    //parameters["envDelay"] = 0;           //Speed of the travelling wave
    parameters["persistency"] = 0;          //Persistency of the species
    parameters["distMean"] = 0.5;           //Mean of species niche, mean of the distribution used for environment generation
    parameters["distVar"] = 0.5;            //Variance of the distribution used for environment generation
    int nIter = 60;                         //Number of iteration in the simulation
    bool plot = true;                       //If you want to plot the results
    string envGeneration = "random";        //Method to generate the environnement : "random", "function"
    string initialRepartition = "pointStart"; //Method to initialize the species repartition : "bottomStart", "oppositeCornerStart", "pointStart"
    string envType = "constant";            //Type of the environment : "constant", "variable"

    //Define the generic filename
    string filename="heterogenous=NormalDist \n "+envType+"\n ";
    
    /*
    //Creation of the species' niches
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({75}));//
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({125}));
    VariableEnv<Vecteur<float>> niche_C(Vecteur<float>({255}));
    VariableEnv<Vecteur<float>> niche_D(Vecteur<float>({0}));

    Vecteur<Vecteur<float>> tol_A({Vecteur<float>({50})});//
    Vecteur<Vecteur<float>> tol_B({Vecteur<float>({100})});
    Vecteur<Vecteur<float>> tol_C({Vecteur<float>({1})});
    Vecteur<Vecteur<float>> tol_D({Vecteur<float>({1})});
    */

    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({75, 39}));//Perforatus
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({125, 255}));//Chthamalus
    VariableEnv<Vecteur<float>> niche_C(Vecteur<float>({255, 255}));//Mask
    VariableEnv<Vecteur<float>> niche_D(Vecteur<float>({0, 255}));//Mask

    Vecteur<Vecteur<float>> tol_A({Vecteur<float>({50, 75}), Vecteur<float>({0, 200})});//
    Vecteur<Vecteur<float>> tol_B({Vecteur<float>({100, 0}), Vecteur<float>({0, 255})});
    Vecteur<Vecteur<float>> tol_C({Vecteur<float>({1, 0}), Vecteur<float>({0, 255})});
    Vecteur<Vecteur<float>> tol_D({Vecteur<float>({1, 0}), Vecteur<float>({0, 255})});

    //Creation of the species*/
    Specie A(niche_A,"A", 2, tol_A);
    Specie B(niche_B,"B", 2, tol_B);
    Specie C(niche_C,"C", 2, tol_C);
    Specie D(niche_D,"D", 2, tol_D);

    //Creation of the species list
    vector<Specie> spVector({A, B, C, D});  

    //Creation of the environment
    //Environment_t E(spVector, parameters, filename, envGeneration, initialRepartition, envType); //Env from functors only
    Environment_t E(depthImage, vegetationImage, spVector, parameters, filename, initialRepartition, envType);           //Env from image and functors
    //Environment_t E(depthImage, spVector, parameters, filename, initialRepartition, envType); 

    //Running simulation
    Simulation automate(E, nIter, plot);

    //Display   
    cout << automate.countVector[0].size() << endl;
    cout << automate.timeBeforeStationarity << endl;

    Vecteur<float> t; for (int i=0; i<automate.timeBeforeStationarity+2; i++){t.push_back(i);}
    plt::plot(t, automate.countVector[0]/(parameters["m"]*parameters["n"]));
    plt::title("Proportion of the specie A (#square A/# tot square)");
    //plt::legend();
    plt::savefig("output/images/popSizes.pdf");
    plt::clf();

    /*
    //Convert repartiton of species into a labelled array for display
    Vecteur<int> labels({1,2});
    Vecteur<int> repartitionLabeled({});
    for (int i=0; i<parameters["n"]; i++)
    {
        if (automate.environment.repartition[i][0].name=="A"){repartitionLabeled.push_back(labels[0]);}
        else {repartitionLabeled.push_back(labels[1]);}
    }

    Vecteur<float> x; for (int i=0; i<parameters["n"]; i++){x.push_back(i);}
    plt::scatter(x, repartitionLabeled);
    plt::title("A distribution");
    plt::legend();
    plt::savefig("output/images/rep_scatter.pdf");
    plt::clf();
    
    
    //Input
    int nReplicate = 1;//20;
    vector<float> means({0.5}); //{0.3,0.4,0.5,0.55,0.56,0.57,0.58,0.59,0.6,0.7,0.8,0.9}
    vector<float> variances({0.5});
    vector<float> persistencies({0});
    int padding = 50;
    
    //output
    Vecteur<Vecteur<float>> allEndMigration(nReplicate, Vecteur<float> (means.size(),0));
    Vecteur<Vecteur<Vecteur<float>>> allPopSize(nReplicate, Vecteur<Vecteur<float>> (means.size(),Vecteur<float> (nIter,0)));
    Vecteur<Vecteur<float>> popSizeMean(means.size(),Vecteur<float> (nIter,0));
    Vecteur<Vecteur<float>> popSizeStdDev(means.size(),Vecteur<float> (nIter,0));
    Vecteur<float> endMigrationMean(means.size(),0);
    Vecteur<float> endMigrationStdDev(means.size(),0);

    for (int l=0; l<nReplicate; l++){

        //Temporary memory
        Vecteur<float> replicateEndMigration;
        Vecteur<Vecteur<float>> replicatePopSize;

        for (int k=0; k<persistencies.size(); k++){
            sf::RenderTexture envRender;
            envRender.create(means.size()*parameters["n"]+(means.size()+1)*padding, variances.size()*parameters["n"]+(variances.size()+1)*padding);
            envRender.clear(sf::Color::White);

            for (int i=0; i<means.size(); i++){
                for (int j=0; j<variances.size(); j++){ 
                    
                    //Creation of the environment
                    map<string, float> param;
                    param["n"] = parameters["n"];
                    param["m"] = parameters["m"];
                    param["distMean"] =  means[i];
                    param["distVar"] = variances[j];
                    param["persistency"] = persistencies[k];

                    //Creation of the environment
                    Environment_t E(spVector, param, filename, "random", "oppositeCornerStart");

                    //Run simulation
                    Simulation automate(E, nIter, plot);

                    //Plot the final repartition
                    tuple<sf::Uint8*, string, string, sf::Color, sf::Color> rep = repartitionToPixel(automate.environment);
                    gridPlot(rep, envRender, automate.environment.m, automate.environment.n, i, j, padding, means[i], variances[j], "Mean", "Var");

                    //Keep memory of the migration end
                    replicateEndMigration.push_back(automate.timeBeforeStationarity);

                    //Do one curve
                    replicatePopSize.push_back(automate.countVector[0]/float(parameters["m"]*parameters["n"]));
                }
                cout << "column " << i << " finished \n";
            }
        
        //Display the result and save to a file
        envRender.display();
        sf::Image finalImage = envRender.getTexture().copyToImage();
        finalImage.saveToFile("/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/output/images/ " + filename + " gridMean&Variance \n persistency=" + to_string(persistencies[k]) + "\n replicat" + to_string(l) + ".png");

        allEndMigration[l] += replicateEndMigration;
        allPopSize[l] += replicatePopSize;
        }
    }

    //Calculate mean
    for (int i=0; i<nReplicate;i++){
        endMigrationMean += (allEndMigration[i]/float(nReplicate));
        for (int j=0; j<means.size(); j++){
            popSizeMean[j] += (allPopSize[i][j]/float(nReplicate));
        }
    }
    
    //Calculate Std deviation
    for (int i=0; i<nReplicate; i++){
        for (int j=0; j<means.size(); j++){
            popSizeStdDev[j] +=  allPopSize[i][j]*allPopSize[i][j] - 2.f*(allPopSize[i][j]*popSizeMean[j]) + (popSizeMean[j]*popSizeMean[j]);
        }
        endMigrationStdDev += allEndMigration[i]*allEndMigration[i] - 2.f*(allEndMigration[i]*endMigrationMean) + (endMigrationMean*endMigrationMean);
    }

    for (int i=0; i<means.size(); i++){
        for (int j=0; j<nIter; j++){
            popSizeStdDev[i][j] = sqrt(popSizeStdDev[i][j]/float(nReplicate-1));
        }
        endMigrationStdDev[i] = sqrt(endMigrationStdDev[i]/float(nReplicate-1));
    }

    //Plot the curves invaderProp vs Mean
    for (int i=0; i<means.size();i++){
        plt::errorbar(t, popSizeMean[i], popSizeStdDev[i],{{"label", "mean="+to_string(means[i])}});
    }
    plt::title("Proportion of A wrt time for different generation mean and a variance of 0.1 and a persistency of 0.");
    plt::legend();
    plt::savefig("output/images/popSize_vs_mean.pdf");
    plt::clf();

    //Plot the migration time vs the generation mean
    plt::errorbar(means,endMigrationMean, endMigrationStdDev);
    plt::title("Number of iteration before the end of migrations");
    plt::savefig("output/images/endMigration_vs_mean.pdf");

    cout << endl << "Finished" << endl;
    */
}
