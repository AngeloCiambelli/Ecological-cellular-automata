#include <iostream>
#include <random>
#include <iomanip>
#include <cmath>
#include "Vecteur.hpp"
#include "VariableEnv.hpp"
#include "Population.hpp"
#include "Environment.hpp"
#include "Simulation.hpp"
#include "Functor.hpp"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

int main(int argc, char *argv[])
{
    //===========================================================================
    //                          Load environmental image
    //                 (Carefull with image sizes, they need to match)
    //===========================================================================

    sf::Image depthImage;
    sf::Image vegetationImage;
    depthImage.loadFromFile("include/roscoff_rocky_beach_resized_full_sea.png");
    vegetationImage.loadFromFile("include/vegetation_cover_1.png");
    sf::Vector2u sizeDepth = depthImage.getSize();
    sf::Vector2u sizeVeg = vegetationImage.getSize();
    if(sizeDepth.x != sizeVeg.x | sizeDepth.y != sizeVeg.y){cout << "image size don't match \n"; exit(1);} //Check before if the two images have the same size

    //===========================================================================
    //                          Define model parameters
    //===========================================================================

    map<string, float> parameters;
    parameters["n"] = sizeDepth.x;              //Number of columns of the lattice
    parameters["m"] = sizeDepth.y;              //Number of rows of the lattice
    int nIter =400;                             //Number of iteration in the simulation
    bool plot = true;                           //Plot the results
    string envGeneration = "percolation";       //Method to generate the environnement : "normal", "function", "percolation" (Not used when using images)
    string initialRepartition = "pointStart";   //Method to initialize the species repartition : "bottomStart", "oppositeCornerStart", "pointStart", "centralStart"
    string envType = "constant";                //Type of the environment : "constant", "variable"

    //Probability of open/close, to use when environnement is a percolation pattern 
    //parameters["percolationProbability"] = 0.1;

    //Parameters of the nomral distribution to use for environmental generation
    //parameters["distMean"] = 0.5;             //Mean of species niche, mean of the distribution used for environment generation
    //parameters["distVar"] = 0.5;              //Variance of the distribution used for environment generation
    
    //Parameters when environnement is made with functions
    //parameters["unit"] = 0.1;                 //Unit of a square
    //parameters["envDilatation"] = 5;          //Oscillation parameter
    //parameters["envDelay"] = 0;               //Speed of the travelling wave

    //===========================================================================
    //                   Creation of the populations
    //===========================================================================

    //Make population' niches
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({7, 3}));        //Perforatus optimum
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({1, 5}));        //Chthamalus optimum
    VariableEnv<Vecteur<float>> niche_C(Vecteur<float>({0, 5}));        //Mask optimum

    Vecteur<Vecteur<float>> tol_A({Vecteur<float>({3, 4}), Vecteur<float>({4, 8})});       //Perforatus tolerance
    Vecteur<Vecteur<float>> tol_B({Vecteur<float>({6, 0}), Vecteur<float>({0, 100})});     //Chthamalus tolerance
    Vecteur<Vecteur<float>> tol_C({Vecteur<float>({0.01, 0}), Vecteur<float>({0, 100})});  //Mask tolerance

    /*
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({7})); 
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({3}));
    VariableEnv<Vecteur<float>> niche_C(Vecteur<float>({0}));

    Vecteur<Vecteur<float>> tol_A({Vecteur<float>({3})});
    Vecteur<Vecteur<float>> tol_B({Vecteur<float>({4.5})});
    Vecteur<Vecteur<float>> tol_C({Vecteur<float>({0.1})});
    */

    /*
    VariableEnv<Vecteur<float>> niche_A(Vecteur<float>({0}));
    VariableEnv<Vecteur<float>> niche_B(Vecteur<float>({1}));

    Vecteur<Vecteur<float>> tol_A({Vecteur<float>({1})});
    Vecteur<Vecteur<float>> tol_B({Vecteur<float>({1})});
    */

    //Creation of the species
    Population A(niche_A, "A", 1, tol_A);
    Population B(niche_B, "B", 1, tol_B);
    Population C(niche_C, "C", 1, tol_C);

    //===========================================================================
    //                      Creation of the environnement
    //===========================================================================

    //Creation of the species list
    vector<Population> spVector({A, B, C}); 

    //Define the generic filename
    string filename="heterogenous=bloscon \n "+envType+"\n ";

    //Construction of the environnement
    Environment_t E(depthImage, vegetationImage, spVector, parameters, filename, initialRepartition, envType);         //Env from two images and functors
    //Environment_t E(spVector, parameters, filename, envGeneration, initialRepartition, envType);                     //Env from functors only
    //Environment_t E(depthImage, spVector, parameters, filename, initialRepartition, envType);                        //Env from an image and functors

    //===========================================================================
    //                              Run simulation
    //===========================================================================
    
    Simulation automate(E, nIter, plot);

    //===========================================================================
    //                                  Display
    //===========================================================================  
    
    /*
    Vecteur<float> t; for (int i=0; i<automate.timeBeforeStationarity+2; i++){t.push_back(i);}    
    plt::plot(t, automate.countVector[0]/(parameters["m"]*parameters["n"]));
    plt::title("Proportion of the Population A (#square A/# tot square)");
    //plt::legend();
    plt::savefig("output/images/popSizes.pdf");
    plt::clf();
    */

    //===========================================================================
    //                               Situational code
    //=========================================================================== 

    /*
    //Convert repartition of species into a labelled array for display
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
    */

    /*
    //Input
    int nReplicate = 10;
    vector<float> means({0.3,0.4,0.5,0.55,0.56,0.57,0.58,0.59,0.6,0.61,0.62,0.7,0.8,0.9});
    vector<float> variances({0});
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
                    param["percolationProbability"] = means[i];
                    //param["distMean"] =  means[i];
                    //param["distVar"] = variances[j];
                    //param["persistency"] = persistencies[k];

                    //Creation of the environment
                    Environment_t E(spVector, param, filename, "percolation", "pointStart");

                    //Run simulation
                    Simulation automate(E, nIter, plot);

                    //Plot the final repartition
                    tuple<sf::Uint8*, string, string, sf::Color, sf::Color> rep = repartitionToPixel(automate.environment);
                    gridPlot(rep, envRender, automate.environment.m, automate.environment.n, i, j, padding, means[i], variances[j], "Mean", "Var");

                    //Keep memory of the migration end
                    replicateEndMigration.push_back(automate.timeBeforeStationarity);

                    //Do one curve
                    //replicatePopSize.push_back(automate.countVector[0]/float(parameters["m"]*parameters["n"]));
                }
                cout << "column " << i << " finished \n";
            }

        //Display the result and save to a file
        envRender.display();
        sf::Image finalImage = envRender.getTexture().copyToImage();
        finalImage.saveToFile("/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/output/images/ " + filename + " gridMean&Variance \n persistency=" + to_string(persistencies[k]) + "\n replicat" + to_string(l) + ".png");

        allEndMigration[l] += replicateEndMigration;
        //allPopSize[l] += replicatePopSize;
        }
    }

    //Calculate mean
    for (int i=0; i<nReplicate;i++){
        endMigrationMean += (allEndMigration[i]/float(nReplicate));
        for (int j=0; j<means.size(); j++){
            //popSizeMean[j] += (allPopSize[i][j]/float(nReplicate));
        }
    }
    
    //Calculate Std deviation
    for (int i=0; i<nReplicate; i++){
        for (int j=0; j<means.size(); j++){
            //popSizeStdDev[j] +=  allPopSize[i][j]*allPopSize[i][j] - 2.f*(allPopSize[i][j]*popSizeMean[j]) + (popSizeMean[j]*popSizeMean[j]);
        }
        endMigrationStdDev += allEndMigration[i]*allEndMigration[i] - 2.f*(allEndMigration[i]*endMigrationMean) + (endMigrationMean*endMigrationMean);
    }
    
    for (int i=0; i<means.size(); i++){
        for (int j=0; j<nIter; j++){
            //popSizeStdDev[i][j] = sqrt(popSizeStdDev[i][j]/float(nReplicate-1));
        }
        endMigrationStdDev[i] = sqrt(endMigrationStdDev[i]/float(nReplicate-1));
    }
    */

    /*
    //Plot the curves invaderProp vs Mean
    for (int i=0; i<means.size();i++){
        plt::errorbar(t, popSizeMean[i], popSizeStdDev[i],{{"label", "mean="+to_string(means[i])}});
    }
    plt::title("Proportion of A wrt time for different generation mean and a variance of 0.1 and a persistency of 0.");
    plt::legend();
    plt::savefig("output/images/popSize_vs_mean.pdf");
    plt::clf();
    */

    /*
    cout << "stationaryTimes: " <<  endMigrationMean;
    cout << "stationaryStdDev: " <<  endMigrationStdDev;

    //Plot the migration time vs the generation mean
    plt::errorbar(means,endMigrationMean, endMigrationStdDev, {{"fmt", "o"}, {"color", "black"}, {"linestyle","-"}});

    // Add a vertical dotted line at x = 0.59274605079210
    double percolation_threshold = 0.59274605079210;
    plt::axvline(percolation_threshold, {{"color", "black"}, {"linestyle", ":"}});
    
    //plt::title("Number of iteration before the end of migrations");
    plt::savefig("output/images/endMigration_vs_mean.pdf");
    */

    cout << endl << "Finished" << endl;
}
