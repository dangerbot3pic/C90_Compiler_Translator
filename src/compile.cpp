#include <iostream>

#include "ast.hpp"

std::vector<std::string>GlobalVars;

extern const Node *parseAST();

int main(int argc, char **argv)
{
    const Node *root = parseAST(); // Parse the input

    Context ctxt;

    // Print out a load of stuff
    std::cout<<"\t .file 1 \"test.c\""<<std::endl;
    std::cout<<"\t.section .mdebug.abi32"<<std::endl;
    std::cout<<"\t.previous"<<std::endl;
    std::cout<<"\t.nan\tlegacy"<<std::endl;
    std::cout<<"\t.module\tfp=xx"<<std::endl;
    std::cout<<"\t.module\tnooddspreg"<<std::endl;
    std::cout<<"\t.abicalls"<<std::endl<<std::endl;

    root->compile(std::cout, ctxt, "$2"); // Compile the AST

    return 0; // Successfully compiled
}
