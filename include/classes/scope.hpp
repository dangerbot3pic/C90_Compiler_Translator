#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <iostream>

#include "ast_head.hpp"
#include "statement.hpp"

// Pointer to a statementlist, is used as the body of functions, loops and if blocks
class Scope: public Node
{
	public:
	std::vector<StatementList*>list;

	Scope() {}

 	void print(std::ostream &dst) const 
	 {
		for (auto it : list )
		{
			 dst<<"{"<<std::endl;
			 it->print(dst);
			 dst<<std::endl<<"}"<<std::endl;
		}
	}

	void addScope(StatementList *stat)
	{
		list.push_back(stat);
	}

	void translate(std::ostream &trans, int offset = 0) const
	{
		for (auto it : list) 
		{
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
        return "scope";
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
			if (it->hasGlobal(id) == true)
			{
				return true;
			}
		}
		return false;
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
