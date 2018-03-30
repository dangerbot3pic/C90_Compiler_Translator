#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <iostream>

#include "ast_head.hpp"
#include "parameter.hpp"
#include "scope.hpp"

// Stores only the function declaration
class FunctionDeclaration : public Node
{
    public:
    std::string type;
    std::string name;
    ParameterList *paramlist;
    Scope *body;


    FunctionDeclaration(std::string in_type = NULL, std::string in_name = NULL, ParameterList *in_paramlist = NULL, Scope *in_body = NULL)
        : type(in_type), name(in_name), paramlist(in_paramlist), body(in_body) {}

    void print(std::ostream &dst) const
    {
        if(!body) std::cout<<type<<" "<<name<<"();"<<std::endl;
        else
        {
          if (!type.empty()) std::cout<<type<<" ";
          dst<<name;
          if (paramlist != NULL) paramlist->print(dst);
          if (body != NULL) body->print(dst);
        }
    }

    void translate(std::ostream &trans, int offset = 0) const
    {
        for(int i = 0; i < offset; i++) trans<<"\t";
        trans<<"def "<<name<<" (";
        if (paramlist != NULL) paramlist->translate(trans);
        trans<<") :"<<std::endl;
        if (body != NULL) body->translate(trans, offset+1);
    }

    void compile(std::ostream &comp, Context &ctxt, std::string destreg) const
    {
        std::string label = name;
        bool pres_use = false;

        ctxt.frameSize = 3; // One word for return address, plus buffers
        if (paramlist != NULL) ctxt.paramNo = paramlist->getsize(); // Number of arguments in function declaration
        if (body != NULL) ctxt.decNo = body->countDeclarations(); // Count number of new declarations in function body
        ctxt.frameSize += ctxt.findMaxArgSize(); // Allocate space for all on stack
        // Need to do this before current function is added to bindings so that it's own arguments aren't considered

        if (ctxt.findMaxArgSize() > 4) ctxt.supersizeArgs = true; // Set flag if more than five arguments

        ctxt.add_toFunction(name, label, ctxt.paramNo);// Add function to FunctionBindings

        if (body == NULL) return; // Don't do anything else for declarations
        ctxt.frameSize += ctxt.paramNo;
        ctxt.frameSize += ctxt.decNo;
        pres_use = true;
        if (pres_use) ctxt.frameSize += 8; // Space to store $16 - $23 on stack. These registers need to be preserved
        ctxt.frameSize += 1; // Save old $fp

        if (ctxt.frameSize % 2 != 0) ctxt.frameSize += 1; // Add padding if needed

        ctxt.frameSize *= 4; // Find number of bytes

        comp<<std::endl;
        // .text stuff
	    comp<<"\t.text\t"<<std::endl;
	    comp<<"\t.align\t2"<<std::endl;
	    comp<<"\t.globl\t"<<name<<std::endl;
	    comp<<"\t.set\tnomips16"<<std::endl;
	    comp<<"\t.set\tnomicromips"<<std::endl;
	    comp<<"\t.ent\t"<<name<<std::endl;
	    comp<<"\t.type\t"<<name<<", @function"<<std::endl;

        comp<<label<<":"<<std::endl; // Label defines start of the function

        comp<<"\t.frame\t$fp, "<<ctxt.frameSize<< ", $31"<<std::endl;
	    comp<<"\t.mask\t0x40000000, -4"<<std::endl;
	    comp<<"\t.fmask\t0x00000000, 0"<<std::endl;
	    comp<<"\t.set\tnoreorder"<<std::endl;
	    comp<<"\t.set\tnomacro"<<std::endl;

        comp<<"\taddiu\t$sp, $sp, -"<<ctxt.frameSize<<std::endl; // Allocate space on stack
        comp<<"\tsw\t$31, "<<ctxt.frameSize-4<<"($sp)"<<std::endl;
        comp<<"\tsw\t$fp, "<<ctxt.frameSize-8<<"($sp)"<<std::endl; // Store previous frame pointer
        comp<<"\tmove\t$fp, $sp"<<std::endl; // Set new $sp

        // Next save $16 - $23 on stack if those registers are used in the program
        if (pres_use)
        {
            for (int i = 0; i < 8; i++) comp<<"\tsw\t$"<<i+16<<", "<<ctxt.frameSize-12-i*4<<"($sp)"<<std::endl;
        }

        if (paramlist != NULL) paramlist->compile(comp, ctxt, destreg); // Check arguments and add to local bindings, and also save arguments locally
        body->compile(comp, ctxt, destreg); // Compile the body of the function

        comp<<ctxt.end_label<<":"<<std::endl; // End of function label

        // Restore $16 - $23
        if (pres_use)
        {
            for (int i = 0; i < 8; i++) comp<<"\tlw\t$"<<i+16<<", "<<ctxt.frameSize-12-i*4<<"($sp)"<<std::endl;
        }

        comp<<"\tmove\t$sp, $fp"<<std::endl;
        comp<<"\tlw\t$31, "<<ctxt.frameSize-4<<"($sp)"<<std::endl;
        comp<<"\tlw\t$fp, "<<ctxt.frameSize-8<<"($sp)"<<std::endl;
        comp<<"\taddiu\t$sp, $sp, "<<ctxt.frameSize<<std::endl;
        comp<<"\tj\t$31"<<std::endl; // Return to previous function
        comp<<"\tnop"<<std::endl;

        // More stuff for .text
        comp<<"\t.set\tmacro"<<std::endl;
        comp<<"\t.set\treorder"<<std::endl;
        comp<<"\t.end\t"<<name<<std::endl;
        comp<<"\t.size\t"<<name<<", .-"<<name<<std::endl;

        ctxt.Reset(); // Reset the context, everything except global bindings is reset
    }

    std::string gettype() const
    {
        return "functiondec";
    }

    std::string  getname() const
    {
        return name;
    }

    std::string getval() const
    {
        return "null";
    }
};


#endif
