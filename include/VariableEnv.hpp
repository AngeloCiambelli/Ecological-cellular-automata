#ifndef DEF_VARIABLE_HPP
#define DEF_VARIABLE_HPP

#include <vector>
#include <iostream>
#include <random>
#include <list>
#include <map>
#include <string>

using namespace std;

//========================================================
//                   Description
//========================================================
//
// Define the class to host environmental conditions.
//
// Define related functions and operators.
//
//========================================================
//                  Class definition
//========================================================

// VariableEnv class definition
template<typename T>
class VariableEnv
{   
    public:
    T parameters;

    //Constructor of environmental object
    VariableEnv(const T& v){parameters=v;};
    VariableEnv(){};
    VariableEnv<T>& operator=(const VariableEnv<T>& v){
        (*this).parameters=v.parameters;
        return (*this);
    }
    bool operator ==(VariableEnv v){
        return(this->parameters==v.parameters);
    }
};

//========================================================
//              External functions
//========================================================

template<typename T>
ostream & operator <<(ostream & out, const VariableEnv<T>& u)
{
    out<< u.parameters;
    return out;
}

#endif