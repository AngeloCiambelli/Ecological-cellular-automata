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
#include "Population.hpp"
#include "Environment.hpp"
#include "Vecteur.hpp"

using namespace std;

//======================================================================
//                            Description
//======================================================================
//
// Repertory of functors used to generate the environnement and compute adaptation scores.
//
//======================================================================
//                    Linear algebra functions toolbox 
//======================================================================

// Perform Cholesky decomposition of a *symmetric positive definite* matrix A
void choleskyDecomposition(const Vecteur<Vecteur<float>> &A, Vecteur<Vecteur<float>> &L){
  int n = A.size();
  L = Vecteur<Vecteur<float>>(n, Vecteur<float>(n, 0.0f));

  for (int i = 0; i < n; ++i){
    for (int j = 0; j <= i; ++j){
      float sum = 0.0f;
      // Calculate the sum for the current element
      for (int k = 0; k < j; ++k){
        sum += L[i][k] * L[j][k];
      }
      if (i == j){
        // Diagonal elements
        L[i][j] = sqrt(A[i][i] - sum);
      } 
      else{
        // Off-diagonal elements
        L[i][j] = (A[i][j] - sum) / L[j][j];
      }
    }
  }
}

// Compute the determinant of A = L^T * L
float determinant(const Vecteur<Vecteur<float>> &L){
  float det = 1.0f;
  for (int i = 0; i < L.size(); ++i){
    det *= L[i][i];
  }
  return det * det; // Determinant of A = (det(L))^2
}

// Solve Ax=b using Cholesky decomposition L of A
Vecteur<float> solveCholesky(const Vecteur<Vecteur<float>> &L, const Vecteur<float> &b){
  int n = b.size();
  Vecteur<float> y(n, 0.0f);
  Vecteur<float> x(n, 0.0f);

  // Forward substitution to solve Ly = b
  for (int i = 0; i < n; ++i){
    float sum = 0.0f;
    for (int j = 0; j < i; ++j){
      sum += L[i][j] * y[j];
    }
    y[i] = (b[i] - sum) / L[i][i];
  }

  // Backward substitution to solve L^T x = y
  for (int i = n - 1; i >= 0; --i){
    float sum = 0.0f;
    for (int j = i + 1; j < n; ++j){
      sum += L[j][i] * x[j];
    }
    x[i] = y[i] - sum;
  }

  return x;
}

//======================================================================
//                           Class envChangeFunctor 
//                          (To modify environnement)
//======================================================================

class envChangeFunctor
{
public:
  VariableEnv<Vecteur<float>> operator()(float unit, float i, float j, float m, float n, float t, float a, float b) const
  {
    VariableEnv<Vecteur<float>> newEnv(Vecteur<float>({0.5f*sin(a*i*unit+b*t)+0.5f})); // *CAREFUL* match dimension with number of env variables
    return VariableEnv(newEnv);
  }
};

//======================================================================
//                           Class repFunctor
//              (To set the initial repartition of species)
//======================================================================

class repFunctor
{
public:
  vector<vector<Population>>& operator()(vector<vector<Population>>& rep, float m, int n, vector<Population> sp, string gen)
  {
    if (gen == "bottomStart"){
      for (int i=0; i<m; i++){
        for (int j=0; j<n; j++){
          if (i==m-1) {rep[i*n+j].push_back(sp[0]);} 
          else {rep[i*n+j].push_back(sp[1]);} 
        }
      }
    }
    
    if (gen == "centralStart"){
      int mid_i = m/2;
      int mid_j = n/2;
      for (int i=0; i<m; i++){
        for (int j=0; j<n; j++){ 
          if ((i==mid_i-1) & (j==mid_j-1)) {rep[i*n+j].push_back(sp[0]);}
          else {rep[i*n+j].push_back(sp[1]);} 
        }
      }
    }

    if (gen == "oppositeCornerStart"){
      rep[(m-1)*n+(n-1)].push_back(sp[1]);
      rep[0].push_back(sp[0]);
    }

    else if (gen == "pointStart"){
      for (int i=0; i<m; i++){
        for (int j=0; j<n; j++){
          rep[i*n+j].push_back(sp[2]); //Mask Population
        }
      }
      
      // Initialize random number generator with a seed to ensure reprodicability
      std::mt19937 rng(1);
      std::uniform_int_distribution<int> dist_n(0, n-1);
      std::uniform_int_distribution<int> dist_m(0, m-1);

      // Place n sub-population uniformally distributed on the grid
      for(int k=0; k<n; k++){
        int i = dist_m(rng);
        int j = dist_n(rng);
        rep[i*n+j].pop_back();
        rep[i*n+j].push_back(sp[0]); 
        
        int i_bis = dist_m(rng);
        int j_bis = dist_n(rng);
        rep[i_bis*n+j_bis].pop_back();
        rep[i_bis*n+j_bis].push_back(sp[1]); 
      }
    }
    return(rep);
  }
};

//======================================================================
//                           Class envFunctor
//(To set the initial environment - determinist and probabilist generation)
//======================================================================

class envFunctor
{
public:
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, map<string,float> parameters, string gen)
  {
    if (gen=="function"){
      for (int i=0; i<parameters["m"]; i++){
        for (int j=0; j<parameters["n"]; j++){
          env[i*parameters["n"]+j].parameters=Vecteur<float>({0});
        }
      }
    }
    
    if (gen=="percolation"){
      std::random_device rd{};
      std::mt19937 gen{rd()};
      for (int i=0; i<parameters["m"]; i++){
        for (int j=0; j<parameters["n"]; j++){
          bernoulli_distribution ber_dist(parameters["percolationProbability"]); //Site percolation critical value 0.59274605079210 from https://arxiv.org/abs/1507.03027
          env[i*parameters["n"]+j].parameters=Vecteur<float>({1.f-float(ber_dist(gen))});
        }
      }
    }

    else if(gen=="normal"){
      for (int i=0; i<parameters["m"]; i++){
        for (int j=0; j<parameters["n"]; j++){
          std::random_device rd{};
          std::mt19937 gen{rd()}; //Using generation 32-bit Mersenne Twister by Matsumoto and Nishimura, 1998 (one of the best)
          normal_distribution<float> dist(parameters["distMean"],sqrt(parameters["distVar"]));
          env[i*parameters["n"]+j].parameters=Vecteur<float>({float(dist(gen)), float(dist(gen)), float(dist(gen))});  
        }
      }
    }
    return(env);
  }

  //Environment from a single image 
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, int m, int n, const sf::Image& image)
  {
    for (int y = 0; y < m; ++y) {
      for (int x = 0; x < n; ++x) {
        sf::Color color = image.getPixel(x, y);
        env[y*n+x].parameters=Vecteur<float>({(-1.f*static_cast<float>(color.r)+255.f)/255.f*15.f});
      }
    }
    return env;
  }

  //Environment from two images
  vector<VariableEnv<Vecteur<float>>>& operator()(vector<VariableEnv<Vecteur<float>>>& env, int m, int n, const sf::Image& image1,  const sf::Image& image2)
  {
    for (int y = 0; y < m; ++y) {
      for (int x = 0; x < n; ++x) {
        sf::Color color1 = image1.getPixel(x, y);
        sf::Color color2 = image2.getPixel(x, y);
        env[y*n+x].parameters=Vecteur<float>({(-1.f*static_cast<float>(color1.r)+255.f)/255.f*15.f, static_cast<float>(color2.r)/255.f*10.f});
      }
    }
    return env;
  }
};

//======================================================================
//                           Class gaussianScore
//  (To compute adaptation score of a species in one place of the grid)
//======================================================================

class gaussianScore 
{
public:
  float operator()(const Vecteur<float> &x, const Population &sp) {
    int k = x.size();
    
    Vecteur<Vecteur<float>> L;
    choleskyDecomposition(sp.tolerance, L);
    float toleranceDet = determinant(L);

    // Compute the normalization constant
    float norm_const = 1.f / pow(2.f * M_PI, k / 2.f) / std::sqrt(toleranceDet);

    // Compute the exponent term
    Vecteur<float> diff = x - sp.niche.parameters;

    Vecteur<float> temp = solveCholesky(L, diff);
    float exponent =  diff|temp;
    exponent *= -0.5f;

    // Compute the Gaussian value
    return norm_const * exp(exponent);
  }
};

//======================================================================
//                           Class adaptationScoreFunctor
// (To compute adaptation score of one species in all places of the grid)
//======================================================================

class adaptationScoreFunctor
{
public:
  Vecteur<float> operator()(const vector<VariableEnv<Vecteur<float>>> &cond, Population sp, int m, int n) {
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