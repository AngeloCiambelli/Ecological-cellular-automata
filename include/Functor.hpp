#ifndef DEF_FUNCTOR_HPP
#define DEF_FUNCTOR_HPP

#include <vector>
#include <iostream>
#include <random>
#include <list>
#include <map>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <string>
#include "VariableEnv.hpp"
#include "Species.hpp"
#include "Environment.hpp"
#include "Vecteur.hpp"

using namespace std;

//======================================================================
//                    Linear algebra functions toolbox 
//======================================================================

//Function to perform Cholesky decomposition
void choleskyDecomposition(const Vecteur<Vecteur<float>> &A, Vecteur<Vecteur<float>> &L) {
    int n = A.size();
    L = Vecteur<Vecteur<float>>(n, Vecteur<float>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= i; ++j) {
            float sum = 0;
            if (j == i) { //Diagonal elements
                for (int k = 0; k < j; ++k) {
                    sum += L[j][k] * L[j][k];
                }
                L[j][j] = sqrt(A[j][j] - sum);
            } else {
                for (int k = 0; k < j; ++k) {
                    sum += L[i][k] * L[j][k];
                }
                L[i][j] = (A[i][j] - sum) / L[j][j];
            }
        }
    }
}

//Function to compute the determinant of an upper triangular matrix
float determinant(const Vecteur<Vecteur<float>> &L) {
    float det = 1.0;
    for (int i = 0; i < L.size(); ++i) {
        det *= L[i][i];
    }
    return det;
}

//Function to solve the system of linear equations using forward and backward substitution
Vecteur<float> solveCholesky(const Vecteur<Vecteur<float>> &L, const Vecteur<float> &b) {
    int n = b.size();
    Vecteur<float> y(n, 0);
    Vecteur<float> x(n, 0);

    //Forward substitution to solve Ly = b
    for (int i = 0; i < n; ++i) {
        float sum = 0;
        for (int j = 0; j < i; ++j) {
            sum += L[i][j] * y[j];
        }
        y[i] = (b[i] - sum) / L[i][i];
    }

    //Backward substitution to solve L^T x = y
    for (int i = n - 1; i >= 0; --i) {
        float sum = 0;
        for (int j = i + 1; j < n; ++j) {
            sum += L[j][i] * x[j];
        }
        x[i] = (y[i] - sum) / L[i][i];
    }

    return x;
}

//======================================================================
//                           Class envFunctor 
//======================================================================

// Functor used to modify the environment
class envChangeFunctor
{
public:
  VariableEnv<Vecteur<float>> operator()(float unit, float i, float j, float m, float n, float t, float a, float b) const
  {
    //Vecteur<float> newEnv({float(0)});
    //float coeff = (10-int(t)%10)*a;
    //if (i <= (n-1)/2){newEnv = Vecteur<float>({float(1)});} 
    VariableEnv<Vecteur<float>> newEnv(Vecteur<float>({0.5f*sin(a*i*unit+b*t)+0.5f}));  // add additional dimensions of the env if needed (nD) //{float((i*unit)/((n-1)*unit))}
    return VariableEnv(newEnv);
  }
};

// Functor used to seed the original environment of species
class repFunctor
{
public:
  vector<vector<Specie>>& operator()(vector<vector<Specie>>& rep, float m, int n, vector<Specie> sp, string gen)
  {
    if (gen == "bottomStart") {
      for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
        {
          if (i==m-1) {rep[i*n+j].push_back(sp[0]);} //Alternated: i%2==1 
          else {rep[i*n+j].push_back(sp[1]);}
          //else if (i==3*(n-1)/4 /*& j==3*(n-1)/4*/){rep[i*n+j].push_back(sp[1]);}
        }
      }
    }

    if (gen == "oppositeCornerStart") {
      rep[(m-1)*n+(n-1)].push_back(sp[1]);
      rep[0].push_back(sp[0]);
    }

    else if (gen == "pointStart") {
      
      
      for (int i=0; i<m; i++){
        for (int j=0; j<n; j++)
        {
          rep[i*n+j].push_back(sp[2]); //Mask species of the sea
        }
      }
      
      std::mt19937 rng(1); // Initialize random number generator with a seed to ensure reproduicability
      std::uniform_int_distribution<int> dist_n(0, n-1);
      std::uniform_int_distribution<int> dist_m(0, m-1);

      for(int k=0; k<n; k++) {
        int i = dist_m(rng);
        int j = dist_n(rng);
        rep[i*n+j].pop_back();
        rep[i*n+j].push_back(sp[0]); 

        int i_bis = dist_m(rng);
        int j_bis = dist_n(rng);
        rep[i_bis*n+j_bis].pop_back();
        rep[i_bis*n+j_bis].push_back(sp[1]); 
        
        int i_bis2 = dist_m(rng);
        int j_bis2 = dist_n(rng);
        rep[i_bis2*n+j_bis2].pop_back();
        rep[i_bis2*n+j_bis2].push_back(sp[3]);
        
      }
    }

    return(rep);
  }
};

// Functor to set the initial environment (determinist and probabilist generation)
class envFunctor
{
public:
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, map<string,float> parameters, string gen)
  {
    if (gen=="function"){
      for (int i=0; i<parameters["m"]; i++){
        for (int j=0; j<parameters["n"]; j++)
        {
          env[i*parameters["n"]+j].parameters=Vecteur<float>({sin(0.5f*float(parameters["envDilatation"]*i*parameters["unit"])+0.5f)});  // add additional dimensions of the env if needed (nD) {float((i*unit)/((n-1)*unit))}
        }
      }
    }
    
    else if(gen=="random"){
      for (int i=0; i<parameters["m"]; i++){
        for (int j=0; j<parameters["n"]; j++)
        {
          std::random_device rd{};
          std::mt19937 gen{rd()}; //Using generation 32-bit Mersenne Twister by Matsumoto and Nishimura, 1998 (one of the best)
          normal_distribution<float> dist(parameters["distMean"],sqrt(parameters["distVar"]));
          env[i*parameters["n"]+j].parameters=Vecteur<float>({float(dist(gen)), float(dist(gen)), float(dist(gen))});  
        }
      }
    }

    return(env);
  }

  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, int m, int n, const sf::Image& image)
  {
    for (int y = 0; y < m; ++y) {
      for (int x = 0; x < n; ++x) {
        sf::Color color = image.getPixel(x, y);
        env[y*n+x].parameters=Vecteur<float>({static_cast<int>(color.r)});
      }
    }

    return env;
  }

  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, int m, int n, const sf::Image& image1,  const sf::Image& image2)
  {
    for (int y = 0; y < m; ++y) {
      for (int x = 0; x < n; ++x) {
        sf::Color color1 = image1.getPixel(x, y);
        sf::Color color2 = image2.getPixel(x, y);
        env[y*n+x].parameters=Vecteur<float>({static_cast<int>(color1.r), static_cast<int>(color2.g)});
      }
    }

    return env;
  }
};

// Functor used to calculate gaussian score
class gaussianScore
{
public:
  float operator()(Vecteur<float> x, Specie sp) {
    int k = x.size();
    
    Vecteur<Vecteur<float>>  toleranceCholesky;
    choleskyDecomposition(sp.tolerance, toleranceCholesky);
    float toleranceDet = determinant(toleranceCholesky);

    // Compute the normalization constant
    float norm_const = 1.0 / pow(2 * M_PI, k / 2.0) / pow(toleranceDet, 2);

    // Compute the exponent term
    Vecteur<float> diff = x - sp.niche.parameters;

    Vecteur<float> temp = solveCholesky(toleranceCholesky, diff);
    float exponent = temp|temp;
    exponent = -0.5 * exponent;

    // Compute the Gaussian value
    return norm_const * exp(exponent);
  }
};

// Functor used to create a adaptation score grid for a species
class adaptationScoreFunctor
{
public:
  Vecteur<float> operator()(const vector<VariableEnv<Vecteur<float>>> &cond, Specie sp, int m, int n) {
    gaussianScore func;
    Vecteur<float> grid(m*n);
    for (int i=0; i<m; i++){
      for (int j=0; j<n; j++){
        grid[i*n+j] = func(cond[i*n+j].parameters, sp);
      }
    }
    return(grid);
  }
};

#endif