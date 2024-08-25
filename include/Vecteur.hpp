#ifndef DEF_VECTEUR_HPP
#define DEF_VECTEUR_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

//===========================================================================
//                          Description
//===========================================================================
//
// Class Vecteur define to be mathématical vectors. 
// We define them using the class vector from C++ standard library.
// we define extra operations.
//
//===========================================================================
//                          Define vector class
//===========================================================================

template <typename T>
class Vecteur : public vector<T>
{
public:

  // Constructeur
  Vecteur(int d = 0, const T &v0 = T()) : vector<T>(d, v0) {} // dim et composantes constantes
  Vecteur(const initializer_list<T> &vs) : vector<T>(vs) {}   // depuis une liste explicite

  // Fonctions membres
  T operator()(int i) const; // valeur    1->dim (indice non testé)
  T &operator()(int i);      // référence 1->dim (indice non testé)

  Vecteur<T> &operator+=(const Vecteur<T> &v); // u += v
  Vecteur<T> &operator-=(const Vecteur<T> &v); // u -= v
  Vecteur<T> &operator+=(const T &x);          // u += x
  Vecteur<T> &operator-=(const T &x);          // u -= x
  Vecteur<T> &operator*=(const T &x);          // u *= x
  Vecteur<T> &operator/=(const T &x);          // u /= x
  float operator|(const Vecteur<T> &v) const;      // u|v
  float norm() const {return (*this)|(*this);};
};  // fin de définition de la classe

//===========================================================================
//                          Fonctions membres
//===========================================================================

template<typename T>
T Vecteur<T>::operator()(int i) const {return this->at(i-1);}

template<typename T>
T& Vecteur<T>::operator()(int i) {return this->at(i-1);}

template<typename T>
Vecteur<T>& Vecteur<T>::operator+=(const Vecteur<T> &v)
{
  int n = (*this).size();
  if (n != v.size())
  {
    cout << "hop hop hop ils n'ont pas la meme taille tes vecteurs";
    exit(1);
  }

  for (int i = 0; i < n; i++)
  {
    (*this)[i] += v[i];
  }
  return *this;
}

template<typename T>
Vecteur<T>& Vecteur<T>::operator-=(const Vecteur<T> &v)
{
  int n = (*this).size();
  if (n != v.size())
  {
    cout << "hop hop hop ils n'ont pas la meme taille tes vecteurs";
    exit(1);
  }

  for (int i = 0; i < n; i++)
  {
    (*this)[i] -= v[i];
  }
  return *this;
}

template<typename T>
Vecteur<T>& Vecteur<T>::operator+=(const T &x)
{
  int n = (*this).size();
  Vecteur<T> a(n, x);

  (*this) += a;

  return *this;
}

template<typename T>
Vecteur<T>& Vecteur<T>::operator-=(const T &x)
{
  int n = (*this).size();
  Vecteur<T> a(n, x);

  (*this) -= a;

  return *this;
}

template<typename T>
Vecteur<T>& Vecteur<T>::operator*=(const T &x)
{
  int n = (*this).size();

  for (int i = 0; i < n; i++)
  {
    (*this)[i] *= x;
  }

  return *this;
} 

template<typename T>
Vecteur<T>& Vecteur<T>::operator/=(const T &x)
{
  int n = (*this).size();

  for (int i = 0; i < n; i++)
  {
    (*this)[i] /= x;
  }

  return *this;
} 

template<typename T>
float Vecteur<T>::operator|(const Vecteur<T> &v) const
{
  int n = (*this).size();
  if (n != v.size())
  {
    cout << "hop hop hop ils n'ont pas la meme taille tes vecteurs (produit scalaire)";
    exit(1);
  }

  T valeur = T();
  for (int i = 0; i < n; i++)
  {
    valeur += (*this)[i] * v[i];
  }
  return valeur;
}

//===========================================================================
//                          Fonctions externes
//===========================================================================

template <typename T>
ostream &operator<<(ostream &out, const vector<T> &v)
{
  out << "[";
  for (int i = 0; i < v.size(); i++)
  {
    if (i != 0)
    {
      out << ",";
    }
    out << v[i];
  }
  out << "]";
  return (out);
};

template <typename T>
Vecteur<T> operator+(const Vecteur<T> &u, const Vecteur<T> &v)
{
  int n = u.size();
  if (n != v.size())
  {
    cout << "hop hop hop ils n'ont pas la meme taille tes vecteurs";
    exit(1);
  }

  Vecteur<T> nouveau(n);

  for (int i = 0; i < n; i++)
  {
    nouveau[i] = u[i] + v[i];
  }
  return nouveau;
} // u + v

template <typename T>
Vecteur<T> operator-(const Vecteur<T> &u, const Vecteur<T> &v)
{
  return (u + (-v));
} // u - v

template <typename T>
Vecteur<T> operator+(const Vecteur<T> &u) { return u; } // +u

template <typename T>
Vecteur<T> operator-(const Vecteur<T> &u)
{
  int n = u.size();
  Vecteur<T> nouveau(n);

  for (int i = 0; i < n; i++)
  {
    nouveau[i] = -u[i];
  }
  return nouveau;
} // +u

template <typename T>
Vecteur<T> operator+(const Vecteur<T> &u, const T &x)
{
  int n = u.size();
  Vecteur<T> nouveau(n);
  Vecteur<T> x_vector(n, x);

  nouveau = u + x_vector;
  return nouveau;
} // u + x

template <typename T>
Vecteur<T> operator-(const Vecteur<T> &u, const T &x) { return u + (-x); } // u - x

template <typename T>
Vecteur<T> operator*(const Vecteur<T> &u, const T &x)
{
  int n = u.size();
  Vecteur<T> nouveau(n);

  for (int i = 0; i < n; i++)
  {
    nouveau[i] = u[i] * x;
  }
  return nouveau;
} // u * x

template <typename T>
Vecteur<T> operator*(const T &x, const Vecteur<T> &u) { return u * x; } // x*u

template <typename T>
Vecteur<T> operator/(const Vecteur<T> &u, const T &x)
{
  int n = u.size();
  Vecteur<T> nouveau(n);

  for (int i = 0; i < n; i++)
  {
    nouveau[i] = u[i] / x;
  }
  return nouveau;
} // u / x

template<typename T>
Vecteur<T> operator*(const Vecteur<T> &u, const Vecteur<T> &v)
{
  int n = u.size();
  Vecteur<T> nouveau(n);

  if (n != v.size())
  {
    cout << "hop hop hop ils n'ont pas la meme taille tes vecteurs";
    exit(1);
  }

  for (int i = 0; i < n; i++)
  {
    nouveau[i] = u[i] * v[i];
  }
  return nouveau;
}

template<typename T>
Vecteur<T> operator/(const Vecteur<T> &u, const Vecteur<T> &v)
{
  int n = u.size();
  Vecteur<T> nouveau(n);

  if (n != v.size())
  {
    cout << "hop hop hop ils n'ont pas la meme taille tes vecteurs";
    exit(1);
  }

  for (int i = 0; i < n; i++)
  {
    nouveau[i] = u[i] / v[i];
  }
  return nouveau;
}
#endif