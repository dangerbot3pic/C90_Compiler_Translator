#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include <string>
#include <iostream>
#include <vector>

#include "ast_head.hpp"

class Parameter : public Node
{
    public:
    std::string type;
    std::string id;

    Parameter (std::string in_type, std::string in_id)
        : type(in_type), id(in_id) {}

    void print(std::ostream &dst) const
	{
        dst<<type<<" "<<id;
	}

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<id<<std::endl;
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const {}

    std::string gettype() const
    {
        return "parameter";
    }

    std::string  getname() const
    {
        return id;
    }

    std::string getval() const
    {
        return "null";
    }
};


class ParameterList : public Node
{
    public:
    std::vector<Parameter*> list;

    ParameterList(){};

    void addParameter(Parameter *state)
    {
        list.push_back(state);
    }

    void print(std::ostream &dst) const
    {
        bool init = false;
        for (auto it : list)
        {
            if (init) dst<<", ";
            else init = true;
            it->print(dst);
        }
    }

    void translate(std::ostream &trans, int offset = 0) const
    {
        bool one = true;
        for (auto it : list)
        {
            if (!one) 
            {
                trans<<", ";
            }
            it->translate(trans);
            one = false;
        }
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        int i = 0;
        for (auto it : list)
        {
            if (ctxt.inLocalBindings(it->getname()))
            {
                std::cerr<<"ERROR: Redeclaration of variable "<<it->getname()<<" in function arguments"<<std::endl;
                exit(1);
            }
            if (i == 26)
            {
                std::cerr<<"ERROR: Too many arguments to be loaded into registers"<<std::endl;
                exit(1);
            }
            ctxt.add_toLocal(it->getname());
            comp<<"\tsw\t$"<<i+4<<", "<<i*4+8<<"($fp)"<<std::endl;
            i++;
        }
    }

    std::string gettype() const
    {
        return "paramlist";
    }

    std::string  getname() const
    {
        return "-1";
    }

    std::string getval() const
    {
        return "null";
    }

    int getsize() const
    {
        return list.size();
    }
};

#endif
