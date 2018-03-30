#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

#include "context.hpp"

// Used to store names of global variables
extern std::vector<std::string>GlobalVars; 

class Node;

typedef const Node *NodePtr;

// Base class from which others inherit
class Node
{
    public:
    Node () {}
    virtual ~Node() {}

    virtual void print(std::ostream &dst) const =0;
    virtual void translate(std::ostream &trans, int offset = 0) const =0;
    virtual void compile(std::ostream &comp, Context &ctxt, std::string destreg) const =0;
    virtual std::string gettype() const =0;
    virtual std::string getname() const =0;
    virtual std::string getval() const =0;
};

#endif
