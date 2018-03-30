#ifndef RETURN_ITEMS_HPP
#define RETURN_ITEMS_HPP

#include <vector>

#include "ast_head.hpp"


class ReturnItems : public Node 
{
    public: 
    std::vector<NodePtr>list;

    ReturnItems() {}

    void addRet(NodePtr ref) 
    {
        list.push_back(ref);
    }

    void print(std::ostream &dst) const 
    {
        for (auto it : list) 
        {
            it->print(dst);
        }
    }

    void translate(std::ostream &trans, int offset = 0) const 
    {
        for (auto it : list) 
        {
            it->translate(trans);
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
        if (list.size() == 1 && list[0]->gettype() == "unaryop") return "unaryop";
        for (auto it : list)
        {
            if (it->gettype() == "binaryexp") return "binaryexp";
        }
        return "return";
    }

    std::string  getname() const 
    {
        if (list.size() == 1 && list[0]->gettype() == "unaryop") return list[0]->getname();
        return "-1";
    }

    std::string getval() const 
    {
        return "null";
    }
};

class ReturnExp : public Node 
{
    public: 
    NodePtr item;
    std::string op;

    ReturnExp (NodePtr ref, const std::string in_op = "a")
        : item(ref), op(in_op) {}

    void print(std::ostream &dst) const 
    {
        if (op != "a") dst<<" "<<op<<" ";
        item->print(dst);
    }

    void translate(std::ostream &trans, int offset = 0) const 
    {
        if (op != "a") trans<<" "<<op<<" ";
        item->translate(trans);
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const 
    {
        item->compile(comp, ctxt, destreg);
    }

    std::string gettype() const 
    {
        if (item->gettype() == "unaryop") return "unaryop";
        if (item->gettype() != "int" || item->gettype() != "variable") return "binaryexp";
        return "returnlist";
    }

    std::string  getname() const 
    {
        if (item->gettype() == "unaryop") return item->getname();
        return "-1";
    }

    std::string getval() const 
    {
        return "null";
    }
};


#endif