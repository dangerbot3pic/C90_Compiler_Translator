#ifndef astprimitives_hpp
#define astprimitives_hpp

#include <string>
#include <iostream>
#include <vector>

#include "ast_head.hpp"

class Variable : public Node
{
public:
	std::string var;
	Variable(const std::string id)
		: var(id) {}

	void print(std::ostream &dst) const
	{
		dst<<var;
	}

	void translate(std::ostream &trans, int offset = 0) const
	{
		for (int i = 0; i < offset; i++) trans<<"\t";
		trans<<var;
	}

	void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
	{
		int ref = 0;
		if (!ctxt.inLocalBindings(var)) // If variable not previously declared locally
		{
			if (ctxt.inGlobalBindings(var))
			{
				comp<<"\tlw\t"<<destreg<<", %got("<<var<<")($28)"<<std::endl;
				comp<<"\tnop"<<std::endl;
				comp<<"\tlw\t"<<destreg<<", 0("<<destreg<<")"<<std::endl;
				comp<<"\tnop"<<std::endl;
				return;
			}
			std::cerr<<"ERROR: Variable "<<var<<" not declared"<<std::endl;
			exit(1);
		}
		else ref = ctxt.findLocalBinding(var);
		comp<<"\tlw\t"<<destreg<<", "<<ref<<"($fp)"<<std::endl;
		comp<<"\tnop"<<std::endl;
	}

	std::string gettype() const
    {
        return "variable";
    }

    std::string  getname() const
    {
        return var;
    }

	std::string getval() const
    {
        return "null";
    }
};


class Pointer : public Node
{
public:
	std::string var;
	Pointer(const std::string id)
		: var(id) {}

	void print(std::ostream &dst) const
	{
		dst<<var;
	}

	void translate(std::ostream &trans, int offset = 0) const
	{
		for (int i = 0; i < offset; i++) trans<<"\t";
		trans<<var;
	}

	void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
	{
		if (!ctxt.inLocalBindings(var)) // If variable not previously declared locally
		{
			std::cerr<<"ERROR: Variable "<<var<<" not declared"<<std::endl;
			exit(1);
		}

		if (!ctxt.inDynamicBindings(var)) ctxt.add_toDynamic(var);
		comp<<"\tlw\t"<<destreg<<", "<<ctxt.findLocalBinding(var)<<"($fp)"<<std::endl;
		comp<<"\tnop"<<std::endl;
		comp<<"\tlw\t"<<destreg<<", 0("<<destreg<<")"<<std::endl;
	}

	std::string gettype() const
    {
        return "pointer";
    }

    std::string  getname() const
    {
        return var;
    }

	std::string getval() const
    {
        return "null";
    }
};


class Reference : public Node
{
	public:
	std::string var;
	Reference(const std::string id)
		: var(id) {}

	void print(std::ostream &dst) const
	{
		dst<<var;
	}

	void translate(std::ostream &trans, int offset = 0) const
	{
		for (int i = 0; i < offset; i++) trans<<"\t";
		trans<<var;
	}

	void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
	{
		if (!ctxt.inLocalBindings(var)) // If variable not previously declared locally
		{
			if (ctxt.inGlobalBindings(var))
			{
				comp<<"\tlui\t"<<destreg<<", %hi("<<var<<")"<<std::endl;
				comp<<"\taddiu\t"<<destreg<<", "<<destreg<<", %lo("<<var<<")"<<std::endl;
				return;
			}
			std::cerr<<"ERROR: Variable "<<var<<" not declared"<<std::endl;
			exit(1);
		}
		comp<<"\taddiu\t"<<destreg<<", $fp, "<<ctxt.findLocalBinding(var)<<std::endl;
	}

	std::string gettype() const
    {
        return "reference";
    }

    std::string  getname() const
    {
        return var;
    }

	std::string getval() const
    {
        return "null";
    }
};


class IntNum : public Node
{
public:
	int val;
	IntNum(const int id)
		: val(id) {}

	void print(std::ostream &dst) const
	{
		dst<<val;
	}

	void translate(std::ostream &trans, int offset = 0) const
	{
		for (int i = 0; i < offset; i++) trans<<"\t";
		trans<<val;
	}

	void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
	{
		comp<<"\tli\t"<<destreg<<", "<<val<<std::endl;
	}

	std::string gettype() const
    {
        return "int";
    }

    std::string  getname() const
    {
        return "-1";
    }

	std::string getval() const
    {
        return std::to_string(val);
    }
};

class DoubleNum : public Node
{
	public:
	double val;

	DoubleNum(const double id)
		: val(id) {}

	void print(std::ostream &dst) const
	{
		dst<<val;
	}

	void translate(std::ostream &trans, int offset = 0) const
	{
		for (int i = 0; i < offset; i++) trans<<"\t";
		trans<<val;
	}

	void compile(std::ostream &comp, Context &ctxt, std::string destreg) const {}

	std::string gettype() const
    {
        return "double";
    }

    std::string  getname() const
    {
        return "-1";
    }

	std::string getval() const
    {
        return "null";
    }
};

class NumberList: public Node {
public:
	std::vector<NodePtr>list;

	 NumberList() {}

	void addNumber(NodePtr Num){
		list.push_back(Num);
	}

	void print(std::ostream &dst) const
  {}

		void translate(std::ostream &trans, int offset = 0) const
		{
			int a=0;
			for (auto it : list)
			{
				if(a!=0) trans<<", ";
				it->translate(trans);
				a++;
			}
		}
			void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
	    {
				int regref = ctxt.getRegister();
				ctxt.lockRegister(regref);
				std::string regname = ctxt.find_RegName(regref);
				int i=0;
				for (auto it : list)
				{
					std::string id=destreg+std::to_string(i);
					ctxt.add_toLocal(id);
					i++;
					it->compile(comp, ctxt, regname);
					comp<<"\tsw\t"<<regname<<", "<<ctxt.findLocalBinding(id)<<"($fp)"<<std::endl;
				}
				  ctxt.freeRegister(regref);
			}

				std::string gettype() const
					{
							return "numlist";
					}

					std::string  getname() const
					{
							return "-1";
					}

				std::string getval() const
					{
							return "null";
					}

};

#endif
