#ifndef LIST_HPP
#define LIST_HPP

#include <vector>
#include <iostream>

#include "ast_head.hpp"

// Stores list of arguments in function calls
class List : public Node
{
    public:
    std::vector<NodePtr>list;

    List() {}

    void addList(NodePtr ref)
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

    void translate (std::ostream &trans, int offset = 0) const
    {
        bool init = false;
        for (auto it : list )
        {
            if (init) trans<<", ";
            init = true;
            it->translate(trans);
        }
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const 
    {
      int i = 4;
      for(auto it : list)
      {
          it->compile(comp, ctxt, "$"+std::to_string(i));
          if (ctxt.supersizeArgs) // If more than 5 arguments then each need to be stored on stack as well
          {
              comp<<"\tsw\t"<<"$"+std::to_string(i)<<", "<<i*4-16<<"($sp)"<<std::endl;
          }
          i++;
      }
    }

    std::string gettype() const
    {
        return "list";
    }

    std::string  getname() const
    {
        return "-1";
    }

    std::string getval() const
    {
        return "null";
    }
    int getsize()
    {
      return list.size();
    }
};

#endif
