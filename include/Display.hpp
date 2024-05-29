#ifndef DEF_DISPLAY_HPP
#define DEF_DISPLAY_HPP

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

//Convert the repartition to a pixel array
template<typename T>
tuple<sf::Uint8*, string, string, sf::Color, sf::Color> repartitionToPixel(Environment_t<T> env){

    //Create the color palette
    Vecteur<Vecteur<int>> colorMap(env.species.size(), Vecteur<int> ({0, 0, 0, 255}));
    for(int i=0 ; i<env.species.size(); i++)
    {
        if (i==0){colorMap[i][0]=100; colorMap[i][1]=175; colorMap[i][2]=50;}
        else if (i==1){colorMap[i][0]=225; colorMap[i][1]=216; colorMap[i][2]=75;}
        else if (i!=0 & i!=1){
            colorMap[i][2]=floor((i+1)*255/env.species.size());
        }
    }

    //Fill in a pixel array with the corresponding colormap
    Vecteur<Vecteur<int>> pixelArray(env.n*env.n, Vecteur<int> ({0, 0, 0, 255}));

    for (int i=0; i<env.n; i++){
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
    sf::Uint8* pixels = new sf::Uint8[env.n * env.n * 4];
    for (unsigned int i = 0; i < env.n * env.n; ++i) {
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
tuple<sf::Uint8*, string, string, sf::Color, sf::Color> envToPixel(vector<VariableEnv<Vecteur<float>>> x, int dimension){

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
        pixelArray[i][1] = (1-(x[i].parameters)[dimension]/xMax) * 255;
        pixelArray[i][2] = (1-(x[i].parameters)[dimension]/xMax) * 255;
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

//Add a legend to the image
void addLegend(sf::RenderTarget& target, const std::string& legendText, sf::Font& font, unsigned int imageHeight, unsigned int imageWidth) 
{
    // Create the text for the legend
    sf::Text legendTextObj;
    legendTextObj.setFont(font);
    legendTextObj.setString(legendText);
    legendTextObj.setCharacterSize(10); // Adjust the size as needed
    legendTextObj.setFillColor(sf::Color::Black);
    
    // Center the text
    float padding = 1.0f;
    legendTextObj.setPosition(padding, imageHeight + padding);

    // Draw legend text
    target.draw(legendTextObj);
}

//Linear interpolation function
sf::Color lerpColor(const sf::Color& start, const sf::Color& end, float t) {
    return sf::Color(
        static_cast<sf::Uint8>(start.r + t * (end.r - start.r)),
        static_cast<sf::Uint8>(start.g + t * (end.g - start.g)),
        static_cast<sf::Uint8>(start.b + t * (end.b - start.b)),
        static_cast<sf::Uint8>(start.a + t * (end.a - start.a))
    );
}

//Add a color bar
void addColorBar(sf::RenderTarget& target, unsigned int imageHeight, unsigned int imageWidth, sf::Font& font, string min, string max, sf::Color lowColor, sf::Color upColor) {
    //Create the color bar
    unsigned int colorBarHeight = 40;
    unsigned int colorBarWidth = 10;
    sf::RectangleShape colorBar(sf::Vector2f(colorBarWidth, colorBarHeight));
    colorBar.setPosition(imageWidth - 40, imageHeight+30); //Leave some padding from the right edge

    //Create the gradient texture
    sf::Image gradientImage;
    gradientImage.create(colorBarWidth, colorBarHeight);
    for (unsigned int y = 0; y < colorBarHeight; ++y) {
        float t = static_cast<float>(y) / colorBarHeight;
        sf::Color color = lerpColor(lowColor, upColor, t); //Interpolate between lower color and upper color
        for (unsigned int x = 0; x < colorBarWidth; ++x) {
            gradientImage.setPixel(x, colorBarHeight - y - 1, color); //Reverse the gradient direction
        }
    }
    sf::Texture gradientTexture;
    gradientTexture.loadFromImage(gradientImage);
    colorBar.setTexture(&gradientTexture);

    target.draw(colorBar);

    //Create the labels for the color bar
    sf::Text minLabel, maxLabel;
    minLabel.setFont(font);
    maxLabel.setFont(font);
    minLabel.setString(min);
    maxLabel.setString(max);
    minLabel.setCharacterSize(10);
    maxLabel.setCharacterSize(10);
    minLabel.setFillColor(sf::Color::Black);
    maxLabel.setFillColor(sf::Color::Black);

    //Position the labels
    minLabel.setPosition(imageWidth - 25, colorBarHeight - minLabel.getCharacterSize()+ imageHeight + 30); // Bottom of the color bar
    maxLabel.setPosition(imageWidth - 25, imageHeight + 30); // Top of the color bar

    //Draw the labels
    target.draw(minLabel);
    target.draw(maxLabel);
}

//Save an image
void imagePlot(tuple<sf::Uint8*, string, string, sf::Color, sf::Color> pixels, int t, const std::string& filename, int n) 
{
    //Working directory
    std::string path = "/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/";

    //Create an SFML texture and sprite from the pixel array
    sf::Texture texture;
    texture.create(n, n);
    texture.update(get<0>(pixels));
    sf::Sprite sprite;
    sprite.setTexture(texture);

    //Create an SFML render texture to draw everything, including extra space for the legend
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(n, n + 90)) {
        delete[] get<0>(pixels);
        return; // Error creating render texture
    }

    //Draw the sprite (the image)
    renderTexture.clear(sf::Color::White);
    renderTexture.draw(sprite);

    //Load a font
    sf::Font font;
    if (!font.loadFromFile(path + "lib/AnonymousProMinus/Anonymous Pro Minus.ttf")) {
        delete[] get<0>(pixels);
        return; // Error loading font
    }

    //Create and add the title and legend
    std::string legendText = " " + filename;
    addLegend(renderTexture, legendText, font, n, n);

    //Add the color bar legend
    addColorBar(renderTexture, n, n, font, get<1>(pixels), get<2>(pixels), get<3>(pixels), get<4>(pixels));

    //Display the result and save to a file
    renderTexture.display();
    sf::Image finalImage = renderTexture.getTexture().copyToImage();
    finalImage.saveToFile(path + "output/images/ " + filename);

    //Clean up
    delete[] get<0>(pixels);
}

//Merge multiple images side by side
void mergeImage(string image1, string image2, string image3, string finalFileName){
    //Load textures
    sf::Texture texture1;
    sf::Texture texture2;
    sf::Texture texture3;

    //Working directory
    string path = "/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/";

    texture1.loadFromFile(path+"output/images/ "+image1);
    texture2.loadFromFile(path+"output/images/ "+image2);
    texture3.loadFromFile(path+"output/images/ "+image3);
    
    //Create sprites
    sf::Sprite sprite1(texture1);
    sf::Sprite sprite2(texture2);
    sf::Sprite sprite3(texture3);

    //Determine the size of the final image
    unsigned int width = texture1.getSize().x + texture2.getSize().x + texture3.getSize().x;
    unsigned int height = std::max({texture1.getSize().y, texture2.getSize().y, texture3.getSize().y});

    //Create a render texture
    sf::RenderTexture renderTexture;
    renderTexture.create(width, height);

    //Draw the sprites onto the render texture
    renderTexture.clear();
    sprite1.setPosition(0, 0);
    sprite2.setPosition(texture1.getSize().x, 0);
    sprite3.setPosition(texture1.getSize().x + texture2.getSize().x, 0);

    renderTexture.draw(sprite1);
    renderTexture.draw(sprite2);
    renderTexture.draw(sprite3);
    renderTexture.display();

    //Get the texture from the render texture
    sf::Texture combinedTexture = renderTexture.getTexture();

    //Save the combined image to a file
    combinedTexture.copyToImage().saveToFile(path+"output/images/ "+ finalFileName);
}

#endif