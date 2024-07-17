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
void imagePlot(tuple<sf::Uint8*, string, string, sf::Color, sf::Color> pixels, int t, const std::string& filename, int m, int n) 
{
    //Working directory
    std::string path = "/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/";

    //Create an SFML texture and sprite from the pixel array
    sf::Texture texture;
    texture.create(n, m);
    texture.update(get<0>(pixels));
    sf::Sprite sprite;
    sprite.setTexture(texture);

    //Create an SFML render texture to draw everything, including extra space for the legend
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(n, m + 100)) {
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
    addLegend(renderTexture, legendText, font, m, n);

    //Add the color bar legend
    addColorBar(renderTexture, m, n, font, get<1>(pixels), get<2>(pixels), get<3>(pixels), get<4>(pixels));

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

//Make a grid image 
void gridPlot(std::tuple<sf::Uint8*, std::string, std::string, sf::Color, sf::Color> pixels, sf::RenderTarget& envRender, int m, int n, int i, 
              int j, int padding, float pVal1, float pVal2, std::string pName1="Parameter1", std::string pName2="Parameter2") 
    {
    // Working directory
    std::string path = "/home/angelo/Documents/Master/MasterMaths/MesProjets/Network_diffusion/";

    // Create an SFML texture from the pixel array and update it
    sf::Texture texture;
    texture.create(n, m);
    texture.update(std::get<0>(pixels));

    // Create a sprite to draw the texture onto the render texture
    sf::Sprite sprite(texture);

    // Calculate the position of the texture within the render texture
    sprite.setPosition(i*n+(i+1)*padding, j*m+(j+1)*padding);
    
    // Start drawing on the render texture
    envRender.draw(sprite);

    // Load a font
    sf::Font font;
    if (!font.loadFromFile(path + "lib/AnonymousProMinus/Anonymous Pro Minus.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        delete[] std::get<0>(pixels);
        return;
    }

    // Create the text for the legend
    sf::Text legendTextObj;
    legendTextObj.setFont(font);
    legendTextObj.setCharacterSize(14); // Adjust the size as needed
    legendTextObj.setFillColor(sf::Color::Black);

    //Convert the values 1 and 2 with a certain certaincy on display. 
    std::ostringstream valStream1; valStream1 << std::fixed << std::setprecision(2) << pVal1;
    std::ostringstream valStream2; valStream2 << std::fixed << std::setprecision(2) << pVal2;

    if (i == 0 && j == 0) {
        sf::Text legendText1(pName1 + ":" + valStream1.str(), font, 10);
        legendText1.setFillColor(sf::Color::Black);
        legendText1.setCharacterSize(14); // Adjust the size as needed
        legendText1.setPosition(padding + n / 2, padding / 2);
        envRender.draw(legendText1);

        sf::Text legendText2(pName2 + ":\n" + valStream2.str(), font, 10);
        legendText2.setFillColor(sf::Color::Black);
        legendText2.setCharacterSize(14); // Adjust the size as needed
        legendText2.setPosition(padding/4, padding + m / 2);
        envRender.draw(legendText2);
    } else if (i == 0 && j != 0) {
        legendTextObj.setString(valStream2.str());
        legendTextObj.setPosition(padding/4, j * (padding + n) + padding + m/ 2);
        envRender.draw(legendTextObj);
    } else if (i != 0 && j == 0) {
        legendTextObj.setString(valStream1.str());
        legendTextObj.setPosition(i * (padding + n) + padding/2 + n / 2, padding / 2);
        envRender.draw(legendTextObj);
    }

    // Clean up
    delete[] std::get<0>(pixels);
}

#endif