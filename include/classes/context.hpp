#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <algorithm>

static int unique_no = 0;

// Only used with when compiling, the compile function. Ignore when looking at only translate or print
class Context
{
    public:
    bool reg[32];
    int paramNo;
    int frameSize;
    int decNo;
    int argNo;

    std::string end_label; // The stores the label at the end of the function
    std::string if_start;
    std::string else_start;
    std::string if_end;

    bool supersizeArgs;

    std::unordered_map<std::string, int>GlobalBindings; // Stores all global variables
    std::unordered_map<std::string, int>LocalBindings; // Stores local variables for each function
    std::unordered_map<std::string, int>FunctionBindings; // Stores functions that have been declared, their label and number of arguments required
    std::vector<std::string> DynamicBindings;

    ~Context () {}

    Context ()
    {
        paramNo = 0;
        frameSize = 0;
        decNo = 0;
        argNo = 4;
        supersizeArgs = false;
        for (int i = 0; i < 32; i++) reg[i] = false;
        // Set registers that cannot be used by programmer to true, as they cannot be directly changed
        reg[0] = true; // $0 is hardwired to 0. Is $zero
        reg[1] = true; // $1 holds assembler temporaries, not for programmer use. Is $at
        reg[2] = true; // $2 is used to store the return value
        reg[3] = true; // $3 is used to store the second part of the return value if return is a double
        reg[4] = true; // registers $4 - $7 are used to store arguments
        reg[5] = true;
        reg[6] = true;
        reg[7] = true;
        reg[26] = true; // Reserved by OS
        reg[27] = true; // Reserved by OS
        reg[28] = true; // Global Pointer
        reg[29] = true; // Stack Pointer
        reg[30] = true; // Frame Pointer
        reg[31] = true; // Return address
        // All other registers are free to use
        // $16 - $23 must be saved if used, as they must be preserved

        end_label = makeUnique("end");
    }

    Context(Context *c)
    {
        for (int i = 0; i < 32; i++) reg[i] = c->reg[i];
        paramNo = c->paramNo;
        frameSize = c->frameSize;
        decNo = c->decNo;
        argNo = c->argNo;
        end_label = c->end_label;
        supersizeArgs = c->supersizeArgs;
        GlobalBindings = c->GlobalBindings;
        LocalBindings = c->LocalBindings;
        FunctionBindings = c->FunctionBindings;
    }

    void Reset()
    {
        paramNo = 0;
        frameSize = 0;
        decNo = 0;
        supersizeArgs = false;
        LocalBindings.clear();
        for (int i = 0; i < 32; i++) reg[i] = false;
        reg[0] = true;
        reg[1] = true;
        reg[2] = true;
        reg[3] = true;
        reg[4] = true;
        reg[5] = true;
        reg[6] = true;
        reg[7] = true;
        reg[26] = true;
        reg[27] = true;
        reg[28] = true;
        reg[29] = true;
        reg[30] = true;
        reg[31] = true;
        end_label = makeUnique("end");
    }

    bool inGlobalBindings(const std::string id) const
    {
        if(GlobalBindings.find(id) != GlobalBindings.end()) return true;
        return false;
    }

    void add_toGlobal(const std::string id)
    {
        int loc = GlobalBindings.size();
        GlobalBindings.emplace(id, loc);
    }

    int findGlobalBinding(const std::string id) const
    {
        return (GlobalBindings.find(id)->second)*4;
    }

    bool inLocalBindings(const std::string id) const
    {
        if(LocalBindings.find(id) != LocalBindings.end()) return true;
        return false;
    }

    void add_toLocal(const std::string id)
    {
        int loc = LocalBindings.size();
        LocalBindings.emplace(id, loc);
    }

    int findLocalBinding(const std::string id) const
    {
        return (LocalBindings.find(id)->second+2)*4;
    }

    bool inFunctionBindings(const std::string id, const int args=0) const
    {
        if(FunctionBindings.find(id) != FunctionBindings.end() && FunctionBindings.find(id)->second == args) return true;
        return false;
    }

    void add_toFunction(const std::string id, const std::string label, const int args=0)
    {
        FunctionBindings.emplace(id, args);
    }

    std::string findFunctionBinding(const std::string id) const
    {
        return id;
    }

    bool inDynamicBindings(const std::string in) const 
    {
        if (std::find(DynamicBindings.begin(), DynamicBindings.end(), in) != DynamicBindings.end()) return true;
        return false;
    }

    void add_toDynamic(const std::string in) 
    {
        DynamicBindings.push_back(in);
    }

    std::string makeUnique(const std::string name) const
    {
        return name+std::to_string(unique_no++);
    }

    int getRegister() const
    {
        for (int i = 8; i < 16; i++)
        {
            if (reg[i] == false) return i; // Return the first available register
        }
        if (reg[24] == false) return 24;
        else if (reg[25] == false) return 25;
        else
        {
            for (int i = 16; i < 24; i++) // These registers must be preserved so are used last
            {
                if (reg[i] == false) return i;
            }
        }
        std::cerr<<"ERROR: Registers insufficient to implement program"<<std::endl;
        exit(1);
    }

    void lockRegister(const int i)
    {
        reg[i] = true; // Set the register as in use
    }

    void freeRegister(const int i)
    {
        reg[i] = false; // Free up register for something else to use
    }

    std::string find_RegName(const int i) const
    {
        switch (i)
        {
            case 0: return "$zero";
            case 1: return "$at";
            case 28: return "$gp";
            case 29: return "$sp";
            case 30: return "$fp";
            case 31: return "$ra";
            default: return "$"+std::to_string(i);

        }
    }

    int getStoredRegister()
    {
        for (int i = 16; i < 24; i++) // These registers must be preserved so are used last
        {
            if (reg[i] == false) return i;
        }
        std::cerr<<"ERROR: Registers insufficient to implement program"<<std::endl;
        exit(1);   
    }

    int findMaxArgSize()
    {
        int max = 0;
        for (auto it : FunctionBindings)
        {
            if (max < it.second) max = it.second;
        }
        return max;
    }
};

#endif
