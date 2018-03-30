#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "ast_head.hpp"

class Program : public Node 
{
    public:
    NodePtr left;
    NodePtr right;

    Program (NodePtr le, NodePtr ri = NULL) 
        : left(le), right(ri) 
        {
            if (left != NULL) 
            {
                if (left->gettype() == "declaration") GlobalVars.push_back(left->getname());
            }
            if (right != NULL) 
            {
                if (right->gettype() == "declaration") GlobalVars.push_back(right->getname());
            }
        }

    void print(std::ostream &dst) const
    {
        if (left != NULL) left->print(dst);
        dst<<std::endl;
        if (right != NULL) right->print(dst);
        dst<<std::endl;
    }

    void translate(std::ostream &trans, int offset = 0) const 
    {
        if (left != NULL) left->translate(trans, offset);
        trans<<std::endl;
        if (right != NULL) right->translate(trans, offset);
        trans<<std::endl;
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const 
    {
        if (left != NULL)
        {
            if (left->gettype() == "declaration")
            {
                if (left->getval() == "null")
                {
                    comp<<"\t.comm\t"<<left->getname()<<", 4, 4"<<std::endl;
                }
                else 
                {
                    comp<<"\t.globl "<<left->getname()<<std::endl;
                    comp<<"\t.data"<<std::endl;
                    comp<<"\t.align 2"<<std::endl;
                    comp<<"\t.type "<<left->getname()<<", @object"<<std::endl;
                    comp<<"\t.size "<<left->getname()<<", 4"<<std::endl;
                    comp<<left->getname()<<":"<<std::endl;
                    comp<<"\t.word\t"<<left->getval()<<std::endl;
                }

                if(!ctxt.inGlobalBindings(left->getname())) ctxt.add_toGlobal(left->getname());
                else 
                {
                    std::cerr<<"ERROR: Redeclaration of global variable"<<std::endl;
                    exit(1);
                }
            }
            else left->compile(comp, ctxt, destreg);
        }
        if (right != NULL)
        {
            if (right->gettype() == "declaration")
            {
                if (right->getval() == "null")
                {
                    comp<<"\t.comm\t"<<right->getname()<<", 4, 4"<<std::endl;
                }
                else 
                {
                    comp<<"\t.globl "<<right->getname()<<std::endl;
                    comp<<"\t.data"<<std::endl;
                    comp<<"\t.align 2"<<std::endl;
                    comp<<"\t.type "<<right->getname()<<", @object"<<std::endl;
                    comp<<"\t.size "<<right->getname()<<", 4"<<std::endl;
                    comp<<right->getname()<<":"<<std::endl;
                    comp<<"\t.word\t"<<right->getval()<<std::endl;
                }

                if(!ctxt.inGlobalBindings(right->getname())) ctxt.add_toGlobal(right->getname());
                else 
                {
                    std::cerr<<"ERROR: Redeclaration of global variable"<<std::endl;
                    exit(1);
                }            }
            else right->compile(comp, ctxt, destreg);
        }
    }

    std::string gettype() const 
    {
        return "program";
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