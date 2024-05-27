#ifndef DEF_SIMULATION_HPP
#define DEF_SIMULATION_HPP

#include <vector>
#include <iostream>
#include <random>
#include <list>
#include <cmath>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include "VariableEnv.hpp"
#include "Species.hpp"
#include "Environment.hpp"
#include "Functor.hpp"
#include "Vecteur.hpp"


using namespace std;

//======================================================================
//                           Class Processus 
//======================================================================

template<typename T>
class Simulation
{
    public :
        Environment_t<T>  environment;                                          
        Simulation(const Environment_t<T>& env_init, int nIter, float fecondity, bool plot, string filename); //Constructor of the function 
};

//======================================================================
//                           Member functions
//======================================================================

template<typename T>
Simulation<T>::Simulation(const Environment_t<T>& env_init, int nIter, float fecondity, bool plot, string filename)
{   
    //Initialization
    environment = env_init;
    imagePlot(environment, 0, filename);

    for (int i=1; i<nIter; i++)
    {
        environment=selection(diffusion(environmentalChange(environment, i)));
        
        //Display settings
        if (plot==true){imagePlot(environment, i, filename);}
        else {cout << environment << endl;}
    }
    cout << environment.species;
}

//======================================================================
//                          External functions
//======================================================================

//Diffusion of the species on the grid (determinist)
template<typename T>
Environment_t<T> diffusion(Environment_t<T> env)
{
    //New environment
    Environment_t<T> newEnv(env);

    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            if (env.repartition[i*env.n+j].size()!=0)
            {
                for (int k=0; k<=(env.repartition[i*env.n+j][0]).diffusion_speed; k++){
                    for (int l=0; l<=(env.repartition[i*env.n+j][0]).diffusion_speed-k; l++){
                        if ((i+l < env.n) and (j+k < env.n))
                        {
                        newEnv.repartition[(i+l)*env.n+k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                        if ((i-l >= 0) and (j-k >= 0))
                        {
                        newEnv.repartition[(i-l)*env.n-k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                        if ((i+l < env.n) and (j-k >= 0))
                        {
                        newEnv.repartition[(i+l)*env.n-k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                        if ((i-l >= 0) and (j+k < env.n))
                        {
                        newEnv.repartition[(i-l)*env.n+k+j].push_back(env.repartition[i*env.n+j][0]);
                        }
                    }
                }
            }
        }
    }
    
    return newEnv;
}

//Selection of the best adapted specie in each node of the grid (determinist)
template<typename T>
Environment_t<T> selection(Environment_t<T> env)
{
    //New environment
    Environment_t<T> newEnv(env);

    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            float score(100000);
            int ind(0);
            
            for (int k=0; k<env.repartition[i*env.n+j].size(); k++)
            {
                if ((env.repartition[i*env.n+j][k].niche.parameters - env.conditions[i*env.n+j].parameters).norm() / ((env.conditions[i*env.n+j].parameters).norm()+float(0.0001)) < score)
                {
                    ind = k;
                    score = (env.repartition[i*env.n+j][k].niche.parameters - env.conditions[i*env.n+j].parameters).norm() / ((env.conditions[i*env.n+j].parameters).norm()+float(0.0001));
                }
            }
            
            if (newEnv.repartition[i*env.n+j].size()!=0)
            {
                Vecteur<Specie<T>> bestSp({env.repartition[i*env.n+j][ind]});
                newEnv.repartition[i*env.n+j]=bestSp;
            }
        }
    }
    return newEnv;
}

//Change in the environment according to the functor : f_t(conditions)
template<typename T>
Environment_t<T> environmentalChange(Environment_t<T> env, float t)
{   
    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            envChangeFunctor f;
            env.conditions[i*env.n+j] = f(env.unit, i, j, env.n, t);
        }
    }
    return env;
} 

//Aleatory reproduction of species in each node of the grid chosing two parent species
template<typename T>
Environment_t<T> reproduction(Environment_t<T> env, float fecondity)
{   
    for (int i=0; i<env.n; i++){
        for (int j=0; j<env.n; j++){
            random_device rd;
            mt19937 gen(rd());
            bernoulli_distribution infant(fecondity);
            if (infant(gen)==true)
            {
                uniform_int_distribution<int> parentDistribution{  0, (env.repartition[i*env.n+j]).size()-1};
                Specie<T> parent1(env.repartition[i*env.n+j][parentDistribution(gen)]);
                Specie<T> parent2(env.repartition[i*env.n+j][parentDistribution(gen)]);

                //Ceck if specie already exist in the species list, if not add it
                if ((std::find(env.species.begin(), env.species.end(), mean(parent1,parent2)) != env.species.end())==false)
                {
                    env.species.push_back(mean(parent1,parent2));
                }
                env.repartition[i*env.n+j].push_back(mean(parent1,parent2));
            }  
        }
    }
    return env;
}

//Convert the repartition to a pixel array
template<typename T>
Vecteur<Vecteur<int>> imagePixel(Environment_t<T> env)
{
    //Create the color palette
    Vecteur<Vecteur<int>> colorMap(env.species.size(), Vecteur<int> ({0, 0, 0, 255}));
    for(int i=0 ; i<env.species.size(); i++)
    {
        if (i==0){colorMap[i][0]=100; colorMap[i][1]=175; colorMap[i][2]=50;}
        else if (i==1){colorMap[i][0]=225; colorMap[i][1]=216; colorMap[i][2]=75;}
        else if (i!=0 & i!=1)
        {
            colorMap[i][2]=floor((i+1)*255/env.species.size());
        }
    }

    //Fill in a pixel array with the corresponding colormap
    Vecteur<Vecteur<int>> pixelArray(env.n*env.n, Vecteur<int> ({0, 0, 0, 255}));

    for (int i=0; i<env.n; i++)
    {
        for (int j=0; j<env.n; j++)
        {
            if (env.repartition[i*env.n+j].size()!=0)
            {
                int k = (find(env.species.begin(), env.species.end(), (env.repartition[i*env.n+j][0])) - env.species.begin());
                pixelArray[i*env.n+j][0] = colorMap[k][0];
                pixelArray[i*env.n+j][1] = colorMap[k][1];
                pixelArray[i*env.n+j][2] = colorMap[k][2];
            }   
        }
    }
    
    return(pixelArray);
}

//Add a legend to the image
void addLegend(sf::RenderTarget& target, const std::string& legendText, sf::Font& font) 
{
    // Create the text for the legend
    sf::Text legendTextObj;
    legendTextObj.setFont(font);
    legendTextObj.setString(legendText);
    legendTextObj.setCharacterSize(9);
    legendTextObj.setFillColor(sf::Color::White);

    // Draw the legend
    target.draw(legendTextObj);
}

//Save an image
template<typename T>
void imagePlot(Environment_t<T>& env, int t, std::string filename) 
{
    //Generate pixel array
    Vecteur<Vecteur<int>> pixelArray = imagePixel(env);

    //Working directory
    string path = "/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/";

    //Automatic name
    filename += + "t=" + to_string(t) + "\n ";
    for (int i = 0; i < env.species.size(); i++) {
        filename += env.species[i].name + ":[{" + std::to_string(llround(env.species[i].niche.parameters[0])) + "}," + to_string(env.species[i].diffusion_speed) + "]";
        if (i<env.species.size()-1){filename += "_";}
    }

    //Convert the pixel array to an sf::Uint8 array
    sf::Uint8* pixels = new sf::Uint8[env.n * env.n * 4];
    for (unsigned int i = 0; i < env.n * env.n; ++i) {
        pixels[i * 4] = pixelArray[i][0];     // Red
        pixels[i * 4 + 1] = pixelArray[i][1]; // Green
        pixels[i * 4 + 2] = pixelArray[i][2]; // Blue
        pixels[i * 4 + 3] = pixelArray[i][3]; // Alpha
    }

    //Create an SFML texture and sprite from the pixel array
    sf::Texture texture;
    texture.create(env.n, env.n);
    texture.update(pixels);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    //Create an SFML render texture to draw everything
    sf::RenderTexture renderTexture;
    renderTexture.create(env.n, env.n);

    //Draw the sprite (the image)
    renderTexture.draw(sprite);

    //Load a font
    sf::Font font;
    if (!font.loadFromFile(path+"lib/AnonymousProMinus/Anonymous Pro Minus.ttf")) {
        delete[] pixels;
        return; //Error loading font
    }

    //Create and add the title and legend
    std::string legendText = " " + filename;
    addLegend(renderTexture, legendText, font);

    //Display the result and save to a file
    renderTexture.display();
    sf::Image finalImage = renderTexture.getTexture().copyToImage();
    finalImage.saveToFile(path+"/output/images/" + filename + ".png");

    //Clean up
    delete[] pixels;
}


#endif