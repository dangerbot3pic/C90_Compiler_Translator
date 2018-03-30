#ifndef STAT_TYPES_HPP
#define STAT_TYPES_HPP

#include <iostream>

#include "scope.hpp"
#include "list.hpp"
#include "primitives.hpp"

////////////////////////////////////////////////////////// Return Statements
class Return : public Statement
{
    public:
    NodePtr id;

    Return(NodePtr in_exp)
        : id(in_exp) {}

    void print(std::ostream &dst) const
    {
        dst<<"return ";
        id->print(dst);
        dst<<";";
    }

    void translate(std::ostream &trans, int offset = 0) const
	{
        if (id->gettype() == "unaryop")
        {
            for (int i = 0; i < offset; i++) trans<<"\t";
            id->translate(trans);
            trans<<std::endl;
            for (int i = 0; i < offset; i++) trans<<"\t";
            trans<<"return "<<id->getname()<<std::endl;
        }        
        else 
        {
            for (int i = 0; i < offset; i++) trans<<"\t";
            trans<<"return ";
            id->translate(trans);
        }

	}

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        id->compile(comp, ctxt, destreg);
        comp<<"\tb\t"<<ctxt.end_label<<std::endl;
        comp<<"\tnop"<<std::endl;
    }

    std::string gettype() const
    {
        return "return";
    }

    std::string getname() const
    {
        return "-1";
    }

    std::string getval() const
    {
        return "null";
    }

    int countDeclarations() const
    {
        return 0;
    }

    bool complexReturnItems() const
    {
        if (id->gettype() == "binaryexp") return true;
        return false;
    }
};


////////////////////////////////////////////////////////// Holds single declarations of type 'type x'
class Declaration : public Statement
{
    public:
    std::string type;
    std::string id;
    NodePtr contents;
    bool pointer;

    Declaration (const std::string item_type, const std::string item_id, NodePtr in_contents = NULL, bool in_pointer = false)
        : type(item_type), id(item_id), contents(in_contents), pointer(in_pointer) { }

    void print(std::ostream &dst) const
    {
        dst<<type;
        dst<<" "<<id;
        if (!contents)
        {
            dst<<" = ";
            contents->print(dst);
        }
        dst<<";";
    }

    void translate(std::ostream &trans, int offset = 0) const
	{
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<id<<" = ";
        if (contents != NULL) contents->translate(trans);
        else trans<<"0";
	}

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        if(!ctxt.inLocalBindings(id) && type == "non") // If variable has not been declared
        {
            if(ctxt.inGlobalBindings(id))
            {
                int regref = ctxt.getRegister();
                ctxt.lockRegister(regref);
                std::string regname = ctxt.find_RegName(regref);
                comp<<"\tlw\t"<<destreg<<", %got("<<id<<")($28)"<<std::endl;
                contents->compile(comp, ctxt, regname);
                comp<<"\tsw\t"<<regname<<", 0("<<destreg<<")"<<std::endl;
                return;
            }
            std::cerr<<"ERROR: Undeclared variable "<<id<<std::endl;
            exit(1);
        }
        else if(ctxt.inLocalBindings(id) && type != "non") // If variable is being redeclared in local bindings
        {
          std::cerr<<"ERROR: Redeclaration of variable "<<id<<std::endl;
          exit(1);
        }
        ctxt.add_toLocal(id); // Add variable to local bindings
        // This supports variables of same name and type that has been declared global, and local separately
        // So e.g. int x can be global and local, but only the local version will be used

        if(contents != NULL) // If variable is then assigned a value
        {
            contents->compile(comp,ctxt,destreg);
            if (pointer && contents->gettype() == "binaryexp")
            {
                int reg_ref3 = ctxt.getRegister();
                ctxt.lockRegister(reg_ref3);
                std::string reg_name3 = ctxt.find_RegName(reg_ref3);

                comp<<"\tlw\t"<<reg_name3<<", "<<ctxt.findLocalBinding(id)<<"($fp)"<<std::endl;
                comp<<"\tsw\t"<<destreg<<", 0("<<reg_name3<<")"<<std::endl;
                ctxt.freeRegister(reg_ref3);
                return;
            }
            comp<<"\tsw\t"<<destreg<<", "<<ctxt.findLocalBinding(id)<<"($fp)"<<std::endl;
        }
        
    }

    std::string gettype() const
    {
        if (type == "non") return "decl";
        return "declaration";
    }

    std::string  getname() const
    {
        return id;
    }

    std::string getval() const
    {
        if (contents != NULL) return contents->getval();
        else return "null";
    }

    int countDeclarations() const
    {
        if (type != "non") return 1;
        return 0;
    }

    bool complexReturnItems() const
    {
        return false;
    }
};


/////////////////////////////////////////////////////////// If block stores if else statements. else if's not supported yet
class IfBlock : public Statement
{
    public:
    NodePtr if_condition;
    Scope *if_body;
    Scope *else_body;

    IfBlock(NodePtr in_condition, Scope *in_if_body, Scope *in_else_body)
        : if_condition(in_condition), if_body(in_if_body), else_body(in_else_body) {}

    void print(std::ostream &dst) const
    {
        dst<<"if (";
        if_condition->print(dst);
        dst<<")\n{\n";
        if_body->print(dst);
        dst<<"\n}";
        if (!else_body)
        {
            dst<<"else\n{";
            else_body->print(dst);
            dst<<"\n}";
        }
    }

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<"if (";
        if_condition->translate(trans);
        trans<<") :\n";
        if_body->translate(trans, offset+1);
        if (else_body != NULL)
        {
            for (int i = 0; i < offset; i++) trans<<"\t";
            trans<<"else :\n";
            else_body->translate(trans, offset+1);
        }
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        Context tmp(ctxt); // Create a new context to prevent overwriting of old context variables

        tmp.if_start = tmp.makeUnique("$L"); // Create three unique labels
        tmp.else_start = tmp.makeUnique("$L");
        tmp.if_end = tmp.makeUnique("$L");

        if (if_condition->gettype() != "binaryexp")
        {
            if_condition->compile(comp, tmp, destreg);
            comp<<"\tnop"<<std::endl;
            comp<<"\tbeq\t"<<destreg<<", $0, "<<tmp.else_start<<std::endl;
            comp<<"\tnop"<<std::endl;
        }
        else if_condition->compile(comp, tmp, destreg); // Compile if statement

        comp<<tmp.if_start<<":"<<std::endl;
        if_body->compile(comp, tmp, destreg); // Compile if body
        comp<<"\tnop"<<std::endl;
        comp<<"\tb\t"<<tmp.if_end<<std::endl; // Jump to end of if else statement if body is performed
        comp<<"\tnop"<<std::endl;

        comp<<tmp.else_start<<":"<<std::endl; // Begining of else statement body
        if (else_body != NULL) else_body->compile(comp, tmp, destreg); // Compile else body if it exists
        comp<<tmp.if_end<<":"<<std::endl; // Print out end of if else statement
    }

    std::string gettype() const
    {
        return "ifblock";
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
        int tmp1 = 0;
        int tmp2 = 0;
        if (if_body != NULL) tmp1 = if_body->countDeclarations();
        if (else_body != NULL) tmp2 = else_body->countDeclarations();
        return tmp1+tmp2;
    }

    bool complexReturnItems() const
    {
        return false;
    }
};

/////////////////////////////////////////////////////////// While loops
class WhileBlock: public Statement
{
    public:
    NodePtr while_condition;
    Scope *while_body;
    bool do_while;

    WhileBlock(NodePtr in_condition, Scope *in_body, bool in_do_while = false)
        : while_condition(in_condition), while_body(in_body), do_while(in_do_while) {}

    void print(std::ostream &dst) const
    {
        if(do_while==false){
        dst<<"while ";
        while_condition->print(dst);
        dst<<std::endl;
         while_body->print(dst);
        }
        else{
          dst<<"do {"<<std::endl;
          while_body->print(dst);
          dst<<"}while( ";
          while_condition->print(dst);
          dst<<")"<<std::endl;
        }
    }

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";

        trans<<"while ";
        if (while_condition->gettype() != "int") while_condition->translate(trans);
        else trans<<"True";

        trans<<":"<<std::endl;
        while_body->translate(trans, offset+1);
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        Context tmp(ctxt);

        tmp.if_start = tmp.makeUnique("$L"); // Unique labels
        tmp.else_start = tmp.makeUnique("$L");

        if(do_while==false){
        comp<<tmp.if_start<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;

        while_condition->compile(comp, tmp, destreg);

        while_body->compile(comp, tmp, destreg);

        comp<<"\tnop"<<std::endl;
        comp<<"\tb\t"<<tmp.if_start<<std::endl;
        comp<<"\tnop"<<std::endl;

        comp<<tmp.else_start<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;
        }

        else{
        comp<<tmp.if_start<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;
        while_body->compile(comp, tmp, destreg);

        while_condition->compile(comp, tmp, destreg);
        comp<<"\tnop"<<std::endl;
        comp<<"\tb\t"<<tmp.if_start<<std::endl;
        comp<<"\tnop"<<std::endl;

        comp<<tmp.else_start<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;
        }

    }

    std::string gettype() const
    {
        return "whilestatement";
    }

    std::string getname() const
    {
        return "-1";
    }

    std::string getval() const
    {
        return "null";
    }

    int countDeclarations() const
    {
        if (while_body != NULL) return while_body->countDeclarations();
        return 0;
    }

    bool complexReturnItems() const
    {
        return false;
    }
};

class ForLoop : public Statement
{
    public:
      Statement *Declaration;
      NodePtr Condition;
      NodePtr Expression;
      Scope *Body;

      ForLoop(Statement *in_declaration, NodePtr in_condition, NodePtr in_expression, Scope *in_body): Declaration(in_declaration), Condition(in_condition), Expression(in_expression), Body(in_body) {}

      void print(std::ostream &dst) const
      {
          dst<<"For (";
          Declaration->print(dst);
          dst<<";";
          Condition->print(dst);
          dst<<";";
          Expression->print(dst);
          dst<<")"<<std::endl;
          Body->print(dst);
      }

      void translate(std::ostream &trans, int offset = 0) const
      {
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<"for ";
        if(Declaration!=NULL) trans<<Declaration->getname();
        else trans<<Expression->getname();

        trans<<" in range(";


        if(Declaration!=NULL) trans<<Declaration->getval()<<", ";
        else trans<<(Expression->getname())<<", ";
        trans<<Expression->getval()<<", ";
        trans<<Expression->getval();


        trans<<"):"<<std::endl;
        Body->translate(trans, offset+1);
        trans<<std::endl;
      }

      void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
      {
        Context tmp(ctxt);

        tmp.if_start = tmp.makeUnique("$L"); // Unique labels
        tmp.else_start = tmp.makeUnique("$L");
        tmp.if_end = tmp.makeUnique("$L");

        if(Declaration != NULL) Declaration->compile(comp, tmp, destreg);

        comp<<"\tb\t"<<tmp.if_start<<std::endl;
        comp<<"\tnop"<<std::endl;


        comp<<tmp.if_end<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;

        Body->compile(comp, tmp, destreg);
        Expression->compile(comp, tmp, destreg);

        comp<<tmp.if_start<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;

        if (Condition != NULL) Condition->compile(comp, tmp, destreg);

        comp<<"\tb\t"<<tmp.if_end<<std::endl;
        comp<<"\tnop"<<std::endl;

        comp<<tmp.else_start<<":"<<std::endl;
        comp<<"\tnop"<<std::endl;
        }

    int countDeclarations() const
      {
        int tmp1 = 0;
        int tmp2 = 0;
        if (Declaration != NULL) tmp1 = Declaration->countDeclarations();
        if (Body != NULL) tmp2 = Body->countDeclarations();
        return tmp1 + tmp2;
      }

    bool complexReturnItems() const
      {
        return false;
      }

    std::string gettype() const
      {
        return "forloop";
      }

    std::string getname() const
      {
        return "-1";
      }

    std::string getval() const
      {
        return "null";
      }

};

////////////////////////////////////////////////////////// Function calls
class FunctionCall : public Statement
{
    public:
    std::string name;
    List* params;

    FunctionCall(const std::string in_name, List* in_params = NULL)
        : name(in_name), params(in_params) {}

    void print(std::ostream &dst) const
    {
        dst<<name<<" (";
        if (params != NULL) params->print(dst);
        dst<<");"<<std::endl;
    }

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<name<<" (";
        if (params != NULL) params->translate(trans);
        trans<<")";
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
      int size = 0;
      if(params != NULL) size = params->getsize(); // Find number of parameters
      if (ctxt.inFunctionBindings(name, size))
      {

          if(params == NULL)
          {
            comp<<"\tjal "<<name<<std::endl;
            comp<<"\tnop"<<std::endl;
          }
          else
          {
            params->compile(comp, ctxt, destreg);
            comp<<"\tjal "<<name<<std::endl;
            comp<<"\tnop"<<std::endl;
          }
          comp<<"\tmove\t"<<destreg<<", $2"<<std::endl;
      }
      else
      {
          std::cerr<<"ERROR: Undefined function call "<<name<<std::endl;
          exit(1);
          }
    }
    std::string gettype() const
    {
        return "funcall";
    }

    std::string  getname() const
    {
        return name;
    }

    std::string getval() const
    {
        return "null";
    }

    int countDeclarations() const
    {
        return 0;
    }

    bool complexReturnItems() const
    {
        return false;
    }
};

////////////////////////////////////////////////////////// Unary expressions
class UnaryExpression : public Statement
{
    public:
    NodePtr term;
    std::string op;
    NodePtr change;
    bool prefix;

    UnaryExpression(NodePtr in_term, const std::string in_op, NodePtr in_change, bool in_prefix = false)
        : term(in_term), op(in_op), change(in_change), prefix(in_prefix) {}

    void print(std::ostream &dst) const {}

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";
        term->translate(trans, offset);
        if (op == "++" || op == "+=") trans<<" += ";
        else if (op == "--" || op == "-=") trans<<" -= ";
        else if (op == "*=") trans<<" *= ";
        else trans<<" /= ";
        change->translate(trans);
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        bool ptr = false;
        int reg_ref = ctxt.getRegister();
        ctxt.lockRegister(reg_ref);
        std::string reg_name = ctxt.find_RegName(reg_ref);
        int reg_ref1 = ctxt.getRegister();
        ctxt.lockRegister(reg_ref1);
        std::string reg_name1 = ctxt.find_RegName(reg_ref1);
        
        term->compile(comp, ctxt, destreg);
        if (term->gettype() == "pointer") ptr = true;
        change->compile(comp, ctxt, reg_name);

        if (prefix)
        {
            if (op == "++") comp<<"\taddu\t"<<destreg<<", "<<destreg<<", "<<reg_name<<std::endl;
            else if (op == "--") comp<<"\tsubu\t"<<destreg<<", "<<destreg<<", "<<reg_name<<std::endl;
        }
        else if (op == "++") comp<<"\taddu\t"<<reg_name1<<", "<<destreg<<", "<<reg_name<<std::endl;
        else if (op == "--") comp<<"\tsubu\t"<<reg_name1<<", "<<destreg<<", "<<reg_name<<std::endl;
        else if (op == "+=") comp<<"\taddu\t"<<destreg<<", "<<destreg<<", "<<reg_name<<std::endl;
        else if (op == "-=") comp<<"\tsubu\t"<<destreg<<", "<<destreg<<", "<<reg_name<<std::endl;
        else if (op == "*=")
        {
            comp<<"\tmultu\t"<<destreg<<", "<<reg_name<<std::endl;
            comp<<"\tmflo\t"<<destreg<<std::endl;
        }
        else
        {
            comp<<"\tdivu\t"<<destreg<<", "<<reg_name<<std::endl;
            comp<<"\tmflo\t"<<destreg<<std::endl;
        }

        if (ptr) 
        {
            int reg_ref3 = ctxt.getRegister();
            ctxt.lockRegister(reg_ref3);
            std::string reg_name3 = ctxt.find_RegName(reg_ref3);

            comp<<"\tlw\t"<<reg_name3<<", "<<ctxt.findLocalBinding(term->getname())<<"($fp)"<<std::endl;
            comp<<"\tsw\t"<<destreg<<", 0("<<reg_name3<<")"<<std::endl;
            ctxt.freeRegister(reg_ref3);
        }
        else if (ctxt.inLocalBindings(term->getname()))
        {
            if ((op == "++" || op == "--") && !prefix) comp<<"\tsw\t"<<reg_name1<<", "<<ctxt.findLocalBinding(term->getname())<<"($fp)"<<std::endl;
            else comp<<"\tsw\t"<<destreg<<", "<<ctxt.findLocalBinding(term->getname())<<"($fp)"<<std::endl;
        }
        else
        {
            comp<<"\tlw\t"<<reg_name<<", %got("<<term->getname()<<")($28)"<<std::endl;
            if ((op == "++" || op == "--") && !prefix) comp<<"\tsw\t"<<reg_name1<<", 0("<<reg_name<<")"<<std::endl;
            else comp<<"\tsw\t"<<destreg<<", 0("<<reg_name<<")"<<std::endl;
        }

        ctxt.freeRegister(reg_ref);
        ctxt.freeRegister(reg_ref1);
    }

    std::string gettype() const
    {
        return "unaryop";
    }

    std::string  getname() const
    {
        return term->getname();
    }

    std::string getval() const
    {
      if(op=="--" || op=="-=")  return "-"+change->getval();
      else return change->getval();
    }

    int countDeclarations() const
    {
        return 0;
    }

    bool complexReturnItems() const
    {
        return false;
    }
};

class Array: public Statement{
  public:
      std::string type;
      std::string name;
      NodePtr size;
      NodePtr value;
      NodePtr NumList;

      Array(std::string in_type, std::string in_name, NodePtr in_size, NodePtr in_value, NodePtr in_numlist):
      type(in_type), name(in_name), size(in_size), value(in_value), NumList(in_numlist){}

      void print(std::ostream &dst) const {
        dst<<type;
        dst<<" ";
        dst<<name;
        dst<<"[";
        size->print(dst);
        dst<<"]"<<std::endl;
      }

      void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
      {
        if(type != "non" && NumList == NULL )
          {
            int regref = ctxt.getRegister();
    				ctxt.lockRegister(regref);
    				std::string regname = ctxt.find_RegName(regref);
            comp<<"\tli\t"<<regname<<", 0"<<std::endl;

            for(int i=0; i<std::stoi(size->getval());i++)
            {
              std::string id=name+std::to_string(i);
    					ctxt.add_toLocal(id);
              comp<<"\tsw\t"<<regname<<", "<<ctxt.findLocalBinding(id)<<"($fp)"<<std::endl;
            }
          }

        if(type != "non" && NumList != NULL)
          {
            NumList->compile(comp, ctxt, name);
          }

        if(type == "non" && NumList==NULL && size!=NULL)
          {
            std::string id=name+size->getval();

            if(!ctxt.inLocalBindings(id))
            {
              std::cerr<<"Undeclared Array "<<name<<std::endl;
              exit(1);
            }

            value->compile(comp,ctxt,destreg);
            comp<<"\tsw\t"<<destreg<<", "<<ctxt.findLocalBinding(id)<<"($fp)"<<std::endl;
          }
          if(type == "non" && NumList==NULL && size==NULL)
          {
            std::string id=name+value->getval();
            if(!ctxt.inLocalBindings(id))
            {
              std::cerr<<"Undeclared Array "<<name<<std::endl;
              exit(1);
            }
            comp<<"\tlw\t"<<destreg<<", "<<ctxt.findLocalBinding(id)<<"($fp)"<<std::endl;
          }
      }

      void translate(std::ostream &trans, int offset = 0) const
      {

        for (int i = 0; i < offset; i++) trans<<"\t";

        if(type != "non" && NumList == NULL )
        {
        trans<<name;
        trans<<" = [";
        for(int i=0; i<std::stoi(size->getval());i++)
        {
          if(i!=0) trans<<",";
          trans<<"0";
        }
        trans<<"]"<<std::endl;
        }

        if(type != "non" && NumList != NULL)
        {
          trans<<name;
          trans<<" = [";
          NumList->translate(trans);
          trans<<"];"<<std::endl;
          }

        if(type == "non" && NumList==NULL && size!=NULL)
        {
          trans<<name;
          trans<<"[";
          size->translate(trans);
          trans<<"]=";
          value->translate(trans);
          trans<<std::endl;
        }
        if(type == "non" && NumList==NULL && size==NULL)
        {
          trans<<name;
          trans<<"[";
          value->translate(trans);
          trans<<"]"<<std::endl;
        }
      }

      std::string gettype() const {
        return "null";
      }

      std::string getval() const {
        return "null";
      }

      int countDeclarations() const {
        if(type != "non") return std::stoi(size->getval());
        return 0;
      }

      std::string  getname() const {
        return "null";
      }

      bool complexReturnItems() const {
        return false;
      }
};


///////////////////////////////////////////////// atof function
class AtoF : public Statement
{
    public: 
    std::string var;
    bool variable;

    AtoF(const std::string in_var)
        : var(in_var) {}

    void print (std::ostream &dst) const {}

    void translate(std::ostream &trans, int offset = 0) const
    {
        for (int i = 0; i < offset; i++) trans<<"\t";
        trans<<var;
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const {}

    std::string gettype() const
    {
        return "atof";
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
        return 0;
    }

    bool complexReturnItems() const
    {
        return false;
    }
};

#endif
