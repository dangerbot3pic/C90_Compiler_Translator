#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include <iostream>

#include "ast_head.hpp"

///////////////////////////////////////////////////////////////// Stores all arithmetic expressions, comparison expressions (ie. <, >, != etc.) and boolean logic expressions (&& and ||)
class BinaryExpression : public Node
{
    public:
    NodePtr left;
    std::string op;
    NodePtr right;

    BinaryExpression(NodePtr new_left_term, const std::string new_op, NodePtr new_right_term)
        : left(new_left_term), op(new_op), right(new_right_term) {}

    void print(std::ostream &dst) const
    {
        left->print(dst);
        dst<<" "<<op<<" ";
        right->print(dst);
        dst<<std::endl;
    }

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<"(";
        left->translate(trans);
        trans<<" "<<op<<" ";
        right->translate(trans);
        trans<<")";
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        if (left->gettype() == "int" && right->gettype() == "int" && (op == "+" || op == "-" || op == "/" || op == "*"))// If simple case of integer op integer
        {
                int a1 = std::strtod(left->getval().c_str(), NULL);
                int a2 = std::strtod(right->getval().c_str(), NULL);
                int res = 0;
                if (op == "+") res = a1 + a2;
                else if (op == "*") res = a1*a2;
                else if (op == "/") res = a1/a2;
                else if (op == "-") res = a1 - a2;
                else if (op == "%") res = a1 % a2;
                comp<<"\tli "<<destreg<<", "<<res<<std::endl;
        }
        if (op == "+" || op == "-" || op =="*" || op == "/" || op == "%") // Arithmetic Expression
        {
            int reg_ref1 = 0;
            if (right->gettype() == "funcall") reg_ref1 = ctxt.getStoredRegister();
            else reg_ref1 = ctxt.getRegister(); // Get  a register
            ctxt.lockRegister(reg_ref1); // Lock register
            std::string reg_name1 = ctxt.find_RegName(reg_ref1); // Find name register
            int reg_ref2 = ctxt.getRegister();
            ctxt.lockRegister(reg_ref2);
            std::string reg_name2 = ctxt.find_RegName(reg_ref2);

            left->compile(comp, ctxt, reg_name1); // Compile both branches
            right->compile(comp, ctxt, reg_name2);

            if (op == "+") comp<<"\taddu\t"<<destreg<<", "<<reg_name1<<", "<<reg_name2<<std::endl;
            else if (op == "-") comp<<"\tsubu\t"<<destreg<<", "<<reg_name1<<", "<<reg_name2<<std::endl;
            else if (op == "*")
            {
                comp<<"\tmultu\t"<<reg_name1<<", "<<reg_name2<<std::endl;
                comp<<"\tmflo\t"<<destreg<<std::endl;
            }
            else if (op == "/")
            {
                comp<<"\tdivu\t"<<reg_name1<<", "<<reg_name2<<std::endl;
                comp<<"\tmflo\t"<<destreg<<std::endl;
            }
            else 
            {
                comp<<"\tdivu\t"<<reg_name1<<", "<<reg_name2<<std::endl;
                comp<<"\tmfhi\t"<<destreg<<std::endl;
            }

            ctxt.freeRegister(reg_ref1);
            ctxt.freeRegister(reg_ref2);
        }

        else if (op == "<" || op == ">"  || op == "!=" || op == "==" || op == "<=" || op == ">=") // If comparison expression
        {
            int reg_ref = ctxt.getRegister(); // Allocate a register
            ctxt.lockRegister(reg_ref);
            std::string reg_name = ctxt.find_RegName(reg_ref);

            left->compile(comp, ctxt, destreg); // Compile both sides of the comparison
            right->compile(comp, ctxt, reg_name);
            comp<<"\tnop"<<std::endl;

            if (op == "<") // Print out approproate instructions for op
            {
                comp<<"\tslt\t"<<destreg<<", "<<destreg<<", "<<reg_name<<std::endl;
                comp<<"\tbeq\t"<<destreg<<", $0, "<<ctxt.else_start<<std::endl;
            }
            else if (op == ">")
            {
                comp<<"\tslt\t"<<destreg<<", "<<reg_name<<", "<<destreg<<std::endl;
                comp<<"\tbeq\t"<<destreg<<", $0, "<<ctxt.else_start<<std::endl;
            }
            else if (op == "==") comp<<"\tbne\t"<<destreg<<", "<<reg_name<<", "<<ctxt.else_start<<std::endl;
            else if (op == "!=") comp<<"\tbeq\t"<<destreg<<", "<<reg_name<<", "<<ctxt.else_start<<std::endl;
            else if (op == "<=")
            {
                comp<<"\tsubu\t"<<destreg<<", "<<reg_name<<", "<<destreg<<std::endl;
                comp<<"\tbltz\t"<<destreg<<", "<<ctxt.else_start<<std::endl;
            }
            else
            {
                comp<<"\tsubu\t"<<destreg<<", "<<destreg<<", "<<reg_name<<std::endl;
                comp<<"\tbltz\t"<<destreg<<", "<<ctxt.else_start<<std::endl;
            }
            comp<<"\tnop"<<std::endl;

            ctxt.freeRegister(reg_ref);
        }

        else // Boolean logic statement with && or ||
        {
            if (op == "or") // Logical Or
            {
                left->compile(comp, ctxt, destreg); // Compile left branch
                comp<<"\tb\t"<<ctxt.if_start<<std::endl; // If branch condition is true then jump to start of if statement
                comp<<"\tnop"<<std::endl; // If left branch is false, then test right branch condition
                right->compile(comp, ctxt, destreg); // Compile right branch
            }
            else // Logical And
            {
                left->compile(comp, ctxt, destreg); // Compile left branch
                right->compile(comp, ctxt, destreg); // Compile right branch
                comp<<"\tnop"<<std::endl;
                // If one branch fails then program jumps to else statement
                // Both must pass to continue with if body
            }
        }
    }

    std::string gettype() const
    {
        return "binaryexp";
    }

    std::string  getname() const
    {
        return "-1";
    }

    std::string getval() const
    {
        return right->getval();
    }
};

#endif
