#include <iostream>

#include "ast.hpp"

std::vector<std::string>GlobalVars;

extern const Node *parseAST();

int main(int argc, char **argv)
{
    const Node *root = parseAST(); // Parse the input

    root->translate(std::cout); // Translate the AST to Python
    std::cout<<std::endl;

    // Boilerplate stuff
    std::cout<<"# Boilerplate"<<std::endl;
    std::cout<<"if __name__ == \"__main__\":"<<std::endl;
    std::cout<<"\timport sys"<<std::endl;
    std::cout<<"\tret=main()"<<std::endl;
    std::cout<<"\tsys.exit(ret)"<<std::endl;

    return 0; // Successfully translated
}
