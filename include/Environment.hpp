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
#include "Population.hpp"
#include "Functor.hpp"
#include <SFML/Graphics.hpp>
#include <filesystem>
#include "Display.hpp"


using namespace std;

//======================================================================
//Class Environment_t at time t definition
//======================================================================

class Environment_t
{
    public :
        int n;                                                    //Number of columns of the lattice
        int m;                                                    //Number of rows of the lattice
        float unit;                                               //Unit of the lattice
        float envDilatation;                                      //Parameter of dilatation to set or change the environment
        float envDelay;                                           //Parameter of delay to set or change the environment
        float distMean;                                           //Distribution mean used if the environment have random generation
        float distVar;                                            //Distribution variance used if the environment have random generation
        float percolationProbability;                             //Percolation Probability used to generate the environnement (if used)
        string name;                                              //Name of the environnment with its characteristics
        string envType;

        vector<VariableEnv<Vecteur<float>>> conditions;           //Environmental matrix
        vector<Population> species;                                   //List of species that live in the Environment_t
        vector<vector<Population>> repartition;                       //Species repartition matrix
        vector<int> numberOfChanges;                              //Number of changes in the occupancy for every spot in the lattice
        map<string, Vecteur<float>> adaptationScores;             //Species adaptation scores
        
        //Constructors
        Environment_t(){};                                                                                                    //Empty constructor
        Environment_t(vector<Population> sp, map<string,float> parameters, string filename, string GenType, string repType, string variability);      //Constructor of an environment matrix using functors for initial species repartition and environmental conditions 
        Environment_t(const sf::Image& image, vector<Population> sp, map<string,float> parameters, string filename, string repType, string variability); //Constructor of an environment matrix using image for environmental conditions
        Environment_t(const sf::Image& image1, const sf::Image& image2, vector<Population> sp, map<string,float> parameters, string filename, string repType, string variability);

        //Member functions
        Environment_t migration();
        Environment_t selection();
        Environment_t environmentalChange(float t);
        Vecteur<float> countPopulations();
        void display(string type, int envDimension);
        void display(string type, int envDimension, int i);

        //Operators
        bool operator ==(const Environment_t& env){return(this->repartition==env.repartition);};
        bool operator !=(const Environment_t& env){return(this->repartition!=env.repartition);};
};

//======================================================================
//External functions/operators
//======================================================================

//Print an envrionment in the terminal(repartition and conditions)
ostream& operator <<(ostream & out, const Environment_t& E)
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

//make a customed name for the environment with its parameters
string makeName(string filename, map<string,float> parameters, vector<Population> sp){
    
    //Add the parameters to the filename
    for (auto it=parameters.begin(); it!=parameters.end(); ++it){
        std::ostringstream value; value << std::fixed << std::setprecision(1) << it->second;
        filename += it->first + "=" + value.str() + "\n ";
    }

    for (int i = 0; i < sp.size(); i++) {
        std::ostringstream niche_stream; 
        for (int j=0; j<sp[i].niche.parameters.size(); j++){
            niche_stream << std::fixed << std::setprecision(1) << sp[i].niche.parameters[j];
            if (j!=sp[i].niche.parameters.size()-1){niche_stream<<",";}
        }
        filename += sp[i].name + ":[{" + niche_stream.str() + "}," + to_string(sp[i].diffusion_speed) + "]";
        if (i<sp.size()-1){filename += "_";} else {filename+="\n ";}
    }
    
    return(filename);
}

//Convert the repartition to a pixel array
tuple<sf::Uint8*, string, string, sf::Color, sf::Color> repartitionToPixel(const Environment_t& env){

    //Create the color palette
    Vecteur<Vecteur<int>> colorMap(env.species.size(), Vecteur<int> ({0, 0, 0, 255}));
    for(int i=0 ; i<env.species.size(); i++)
    {
        if (i==0){colorMap[i][0]=100; colorMap[i][1]=175; colorMap[i][2]=50;}
        else if (i==1){colorMap[i][0]=225; colorMap[i][1]=216; colorMap[i][2]=75;}
        else if (i==2){colorMap[i][0]=150; colorMap[i][1]=175; colorMap[i][2]=100;} //To comment if we want to look at infant
        else if (i!=0 & i!=1){
            colorMap[i][2]=floor((i+1)*255/env.species.size());
        }
    }

    //Fill in a pixel array with the corresponding colormap
    Vecteur<Vecteur<int>> pixelArray(env.m*env.n, Vecteur<int> ({0, 0, 0, 255}));

    for (int i=0; i<env.m; i++){
        for (int j=0; j<env.n; j++){
            if (env.repartition[i*env.n+j].size()!=0){
                int k = (find(env.species.begin(), env.species.end(), (env.repartition[i*env.n+j][0])) - env.species.begin());
                pixelArray[i*env.n+j][0] = colorMap[k][0];
                pixelArray[i*env.n+j][1] = colorMap[k][1];
                pixelArray[i*env.n+j][2] = colorMap[k][2];
            }   
        }
    }

    //Convert the pixel array to an sf::Uint8 array
    sf::Uint8* pixels = new sf::Uint8[env.m * env.n * 4];
    for (unsigned int i = 0; i < env.m * env.n; ++i) {
        pixels[i * 4] = pixelArray[i][0];     // Red
        pixels[i * 4 + 1] = pixelArray[i][1]; // Green
        pixels[i * 4 + 2] = pixelArray[i][2]; // Blue
        pixels[i * 4 + 3] = pixelArray[i][3]; // Alpha
    }
    
    return(tuple<sf::Uint8*, string, string, sf::Color, sf::Color> (pixels, env.species[0].name, env.species[env.species.size()-1].name, sf::Color(colorMap[0][0],colorMap[0][1],colorMap[0][2],colorMap[0][3]), sf::Color(colorMap[colorMap.size()-1][0], colorMap[colorMap.size()-1][1], colorMap[colorMap.size()-1][2], colorMap[colorMap.size()-1][3])));
}

//Convert the vectors to a pixel array
template<typename T1>
tuple<sf::Uint8*, string, string, sf::Color, sf::Color> changeToPixel(vector<T1> x){

    //Fill the with intensity depending on the value of x[i]
    Vecteur<Vecteur<int>> pixelArray(x.size(), Vecteur<int> ({255, 255, 255, 255}));

    //Get the maximum of x to handle intensity levels
    float xMax = *max_element(x.begin(), x.end());
    float xMin = *min_element(x.begin(), x.end());

    //Fill the pixel array
    for (int i=0; i<x.size(); i++){
        pixelArray[i][1] = (1-x[i]/xMax) * 255;
        pixelArray[i][2] = (1-x[i]/xMax) * 255;
    }

    //Convert the pixel array to an sf::Uint8 array
    sf::Uint8* pixels = new sf::Uint8[x.size() * 4];
    for (unsigned int i = 0; i < x.size(); ++i) {
        pixels[i * 4] = pixelArray[i][0];     // Red
        pixels[i * 4 + 1] = pixelArray[i][1]; // Green
        pixels[i * 4 + 2] = pixelArray[i][2]; // Blue
        pixels[i * 4 + 3] = pixelArray[i][3]; // Alpha
    }
    
    return(tuple<sf::Uint8*, string, string, sf::Color, sf::Color> (pixels, to_string(xMin), to_string(xMax), sf::Color(255, 255, 255, 255), sf::Color(255, 0, 0, 255)));
}

//Convert the environmental conditions to a pixel array
tuple<sf::Uint8*, string, string, sf::Color, sf::Color> envToPixel(const vector<VariableEnv<Vecteur<float>>>& x, int dimension){

    //Fill the with intensity depending on the value of x[i]
    Vecteur<Vecteur<int>> pixelArray(x.size(), Vecteur<int> ({255, 255, 255, 255}));

    //Get the maximum and minimum of x to handle intensity levels
    float xMax(-10000);
    float xMin(10000);
    for (int i=0; i<x.size(); i++){
        if ((x[i].parameters)[dimension] < xMin){
            xMin = (x[i].parameters)[dimension];
        }
        if ((x[i].parameters)[dimension] > xMax){
            xMax = (x[i].parameters)[dimension];
        }
    }

    //Fill the pixel array
    for (int i=0; i<x.size(); i++){
        pixelArray[i][1] = (1-abs(x[i].parameters[dimension]-xMin)/abs(xMax-xMin)) * 255;
        pixelArray[i][2] = (1-abs(x[i].parameters[dimension]-xMin)/abs(xMax-xMin)) * 255;
    }

    //Convert the pixel array to an sf::Uint8 array
    sf::Uint8* pixels = new sf::Uint8[x.size() * 4];
    for (unsigned int i = 0; i < x.size(); ++i) {
        pixels[i * 4] = pixelArray[i][0];     // Red
        pixels[i * 4 + 1] = pixelArray[i][1]; // Green
        pixels[i * 4 + 2] = pixelArray[i][2]; // Blue
        pixels[i * 4 + 3] = pixelArray[i][3]; // Alpha
    }
    
    return(tuple<sf::Uint8*, string, string, sf::Color, sf::Color> (pixels, to_string(xMin), to_string(xMax), sf::Color(255, 255, 255, 255), sf::Color(255, 0, 0, 255)));
}


//======================================================================
// Member functions
//======================================================================

//float unitEnv, int m, float a, float b,
//Constructor from functors only
Environment_t::Environment_t(vector<Population> sp, map<string,float> parameters, string filename, string genType, string repType, string variability="variable") : numberOfChanges(int(parameters["m"]*parameters["n"]), 0)
{   
    //Extract the parameters
    n = parameters["n"];    
    m = parameters["m"];               
    if (parameters.find("unit") != parameters.end()){envDilatation = parameters["unit"];} 
    if (parameters.find("envDilatation") != parameters.end()){envDilatation = parameters["envDilatation"];}        
    if (parameters.find("envDelay") != parameters.end()){envDelay = parameters["envDelay"];}
    if (parameters.find("distMean") != parameters.end()){distMean = parameters["distMean"];} 
    if (parameters.find("distVar") != parameters.end()){distVar = parameters["distVar"];} 
    if (parameters.find("percolationProbability") != parameters.end()){percolationProbability = parameters["percolationProbability"];} 
    envType = variability;

    //Construction of the environmental matrix
    conditions.resize(m*n);
    envFunctor intialEnv; 
    intialEnv(conditions, parameters, genType);

    //Species
    species = sp;

    //Construction of the intial repartition
    repartition.resize(m*n);
    repFunctor initialRep;
    initialRep(repartition, m, n, sp, repType);

    //Add the parameters and species used in the name of the environment
    name = makeName(filename, parameters, sp);

    //Make the adaptationScore grid of each species if the environment is constant (otherwise useless)
    if(envType=="constant")
    {
        for (int i=0; i<sp.size(); i++)
        {
            adaptationScoreFunctor test;
            adaptationScores.insert({sp[i].name,test(conditions, sp[i], m ,n)});
        }
    }
}

//Constructor from functors and image to set environmental parameters
Environment_t::Environment_t(const sf::Image& image, vector<Population> sp, map<string,float> parameters, string filename, string repType, string variability="variable") : numberOfChanges(int(parameters["m"]*parameters["n"]), 0)
{   
    //Extract the parameters
    n = parameters["n"];    
    m = parameters["m"];
    if (parameters.find("unit") != parameters.end()){envDilatation = parameters["unit"];} 
    if (parameters.find("envDilatation") != parameters.end()){envDilatation = parameters["envDilatation"];}        
    if (parameters.find("envDelay") != parameters.end()){envDelay = parameters["envDelay"];}
    if (parameters.find("distMean") != parameters.end()){distMean = parameters["distMean"];} 
    if (parameters.find("distVar") != parameters.end()){distVar = parameters["distVar"];}
    envType = variability;

    //Construction of the environmental matrix
    conditions.resize(m*n);
    envFunctor intialEnv;
    intialEnv(conditions, m, n, image);

    //Species
    species = sp;

    //Construction of the intial repartition
    repartition.resize(m*n);
    repFunctor initialRep;
    initialRep(repartition, m, n, sp, repType);

    //Add the parameters and species used in the name of the environment
    name = makeName(filename, parameters, sp);

    //Make the adaptationScore grid of each species
    for (int i=0; i<sp.size(); i++)
    {
        adaptationScoreFunctor test;
        adaptationScores.insert({sp[i].name,test(conditions, sp[i], m ,n)});
    }
}

//Constructor from functors and two images to set environmental parameters
Environment_t::Environment_t(const sf::Image& image1, const sf::Image& image2, vector<Population> sp, map<string,float> parameters, string filename, string repType, string variability="variable") : numberOfChanges(int(parameters["m"]*parameters["n"]), 0)
{   
    //Extract the parameters
    n = parameters["n"];    
    m = parameters["m"];
    if (parameters.find("unit") != parameters.end()){envDilatation = parameters["unit"];} 
    if (parameters.find("envDilatation") != parameters.end()){envDilatation = parameters["envDilatation"];}        
    if (parameters.find("envDelay") != parameters.end()){envDelay = parameters["envDelay"];}
    if (parameters.find("distMean") != parameters.end()){distMean = parameters["distMean"];} 
    if (parameters.find("distVar") != parameters.end()){distVar = parameters["distVar"];}
    envType = variability;

    //Construction of the environmental matrix
    conditions.resize(m*n);
    envFunctor intialEnv;
    intialEnv(conditions, m, n, image1, image2);

    //Species
    species = sp;

    //Construction of the intial repartition
    repartition.resize(m*n);
    repFunctor initialRep;
    initialRep(repartition, m, n, sp, repType);

    //Add the parameters and species used in the name of the environment
    name = makeName(filename, parameters, sp);

    //Make the adaptationScore grid of each species
    for (int i=0; i<sp.size(); i++)
    {
        adaptationScoreFunctor test;
        adaptationScores.insert({sp[i].name,test(conditions, sp[i], m ,n)});
    }
}

//Diffusion of the species on the grid (determinist)
Environment_t Environment_t::migration()
{
    //New environment
    Environment_t newEnv(*this);

    for (int i=0; i<this->m; i++){
        for (int j=0; j<this->n; j++){
            if (this->repartition[i*this->n+j].size()!=0)
            {
                for (int k=0; k<=(this->repartition[i*this->n+j][0]).diffusion_speed; k++){
                    for (int l=0; l<=(this->repartition[i*this->n+j][0]).diffusion_speed-k; l++){
                        if ((i+l < this->m) and (j+k < this->n))
                        {
                        newEnv.repartition[(i+l)*this->n+k+j].push_back(this->repartition[i*this->n+j][0]);
                        }
                        if ((i-l >= 0) and (j-k >= 0))
                        {
                        newEnv.repartition[(i-l)*this->n-k+j].push_back(this->repartition[i*this->n+j][0]);
                        }
                        if ((i+l < this->m) and (j-k >= 0))
                        {
                        newEnv.repartition[(i+l)*this->n-k+j].push_back(this->repartition[i*this->n+j][0]);
                        }
                        if ((i-l >= 0) and (j+k < this->n))
                        {
                        newEnv.repartition[(i-l)*this->n+k+j].push_back(this->repartition[i*this->n+j][0]);
                        }
                    }
                }
            }
        }
    }
    
    return newEnv;
}

//Selection of the best adapted Population in each node of the grid (determinist)
Environment_t Environment_t::selection(){

    //New environment
    Environment_t newEnv(*this);

    //If the environnement is variable (changing with time) re-calculate everytime all the scores
    if (this->envType=="variable"){
        for (int i=0; i<this->m; i++){
            for (int j=0; j<this->n; j++){
                if (this->repartition[i*this->n+j].size()!=0){
                    gaussianScore scoreFunction;
                    float score = scoreFunction(this->conditions[i*this->n+j].parameters, this->repartition[i*this->n+j][0]);
                    int ind(0);
                    
                    for (int k=1; k<this->repartition[i*this->n+j].size(); k++){
                        float score_k = scoreFunction(this->conditions[i*this->n+j].parameters, this->repartition[i*this->n+j][k]);
                        if (score_k > score)
                        {
                            ind = k;
                            score = score_k;
                        }
                    }
                    
                    if (newEnv.repartition[i*this->n+j].size()!=0){
                        if (ind!=0)
                        {
                            newEnv.numberOfChanges[i*this->n+j]+=1;
                        }

                        Vecteur<Population> bestSp({this->repartition[i*this->n+j][ind]});
                        newEnv.repartition[i*this->n+j]=bestSp;
                    }
                }
            }
        }
    }

    //If the environnement is constant use pre-calculated scores
    else if (this->envType=="constant"){
        for (int i=0; i<this->m; i++){
            for (int j=0; j<this->n; j++){
                if (this->repartition[i*this->n+j].size()!=0){
                    float score = this->adaptationScores[this->repartition[i*this->n+j][0].name][i*this->n+j];
                    int ind(0);
                    
                    for (int k=1; k<this->repartition[i*this->n+j].size(); k++)
                    {
                        if (this->adaptationScores[this->repartition[i*this->n+j][k].name][i*this->n+j] > score)
                        {
                            ind = k;
                            score = this->adaptationScores[this->repartition[i*this->n+j][k].name][i*this->n+j];
                        }
                        
                    }
                    
                    if (newEnv.repartition[i*this->n+j].size()!=0)
                    {
                        if (ind!=0)
                        {
                            newEnv.numberOfChanges[i*this->n+j]+=1;
                        }

                        Vecteur<Population> bestSp({this->repartition[i*this->n+j][ind]});
                        //cout << bestSp;
                        newEnv.repartition[i*this->n+j]=bestSp;
                    }
                }
            }
        }
    }
    
    return newEnv;
}

//Change in the environment according to the functor : f_t(conditions)
Environment_t Environment_t::environmentalChange(float t){   
    for (int i=0; i<this->m; i++){
        for (int j=0; j<this->n; j++){
            envChangeFunctor f;
            this->conditions[i*this->n+j] = f(this->unit, i, j, this->m, this->n, t, this->envDilatation, this->envDelay);
        }
    }
    return *this;
} 

//Count the number of individuals in each populations
Vecteur<float> Environment_t::countPopulations(){
    Vecteur<float> counts(this->species.size(),0);
    for (int i=0; i<this->m; i++){
        for (int j=0; j<this->n; j++){
            if (this->repartition[i*this->n+j].size()!=0){
                counts[int(find(this->species.begin(), this->species.end(), (this->repartition[i*this->n+j][0])) - this->species.begin())]+=1;
            }
        }
    }
    return counts;
}

//Custom display of the environment
void Environment_t::display(string type, int envDimension){
    tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  repartitionPixels = repartitionToPixel(*this);
    tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  changePixels = changeToPixel(this->numberOfChanges);
    tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  envPixels = envToPixel(this->conditions, envDimension);

    imagePlot(repartitionPixels, 0, this->name+"repartition.png", this->m, this->n);
    imagePlot(changePixels, 0, this->name + "numberChange.png", this->m, this->n);
    imagePlot(envPixels, 0, this->name + "environment dimension "+to_string(envDimension)+".png", this->m, this->n);

    mergeImage(this->name + "environment dimension "+to_string(envDimension)+".png", this->name+"repartition.png", this->name + "numberChange.png", this->name+"merged_t=0.png");
}

//Custom display of the environment with time in the name
void Environment_t::display(string type, int envDimension, int i){

    //Generate pixel array
    tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  repartitionPixels = repartitionToPixel(*this);
    tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  changePixels = changeToPixel(this->numberOfChanges);
    tuple<sf::Uint8*, string, string, sf::Color, sf::Color>  envPixels = envToPixel(this->conditions, envDimension);

    //Filenames
    string repFile = this->name + "repartition \n t=" + to_string(i) + ".png";
    string changeFile = this->name + "numberChange \n t=" + to_string(i) + ".png";
    string envFile = this->name + "environment dimension " + to_string(envDimension) +"\n t=" + to_string(i) + ".png";

    //Save image
    imagePlot(repartitionPixels, i, repFile, this->m, this->n);
    imagePlot(changePixels, i, changeFile, this->m, this->n);
    imagePlot(envPixels, i, envFile, this->m, this->n);

    mergeImage(envFile, repFile, changeFile, this->name+"_merged="+to_string(i)+".png");
}

#endif

