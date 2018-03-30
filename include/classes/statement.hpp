#ifndef STATEMENT_LIST_HPP
#define STATEMENT_LIST_HPP

#include <iostream>
#include <vector>
#include <algorithm>

#include "ast_head.hpp"

class Scope; 

// Base class for pretty much all expressions a C program can use
class Statement : public Node
{
    public: 
    virtual int countDeclarations() const =0; // Find number of new variables declared
    virtual bool complexReturnItems() const =0; // Find if return is of anything other than a variable or number
};

// Stores a vector of statements, used when declaring multiple statements
class StatementList : public Statement
{
    public:
    std::vector<Statement*> list;

    StatementList() {}

    void addStatement(Statement *state)
    {
        list.push_back(state);
    }

    void print(std::ostream &dst) const
	{
		for(auto it : list)
        {
		    it->print(dst);
            dst<<std::endl;
		}
	}

    void translate(std::ostream &trans, int offset = 0) const
    {
        std::vector<std::string>glob;
        for (auto it : list)
        {
            if (it->gettype() == "decl")
            {
                if (std::find(GlobalVars.begin(), GlobalVars.end(), it->getname()) != GlobalVars.end() && std::find(glob.begin(), glob.end(), it->getname()) == glob.end())
                {
                    for (int i = 0; i < offset; i++) trans<<"\t";
                    trans<<"global "<<it->getname()<<std::endl;
                    glob.push_back(it->getname());
                }
            }
            it->translate(trans, offset);
            trans<<std::endl;
        }
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        for (auto it : list)
        {
            it->compile(comp, ctxt, destreg);
        }
    }

    std::string gettype() const
    {
        return "statlist";
    }

    std::string  getname() const
    {
        return "-1";
    }

    std::string getval() const
    {
        return "null";
    }

    int countDeclarations() const
    {
        int count = 0;
        for (auto it : list)
        {
            count += it->countDeclarations();
        }
        return count;
    }

    bool hasGlobal(const std::string id) const
    {
        for (auto it : list)
        {
            if (it->getname() == id)
            {
                if (it->gettype() == "int") return false;
                return true;
            }
        }
    }

    bool complexReturnItems() const
    {
        for (auto it : list)
        {
            if (it->complexReturnItems()) return true;
        }
        return false;
    }
};


#endif
