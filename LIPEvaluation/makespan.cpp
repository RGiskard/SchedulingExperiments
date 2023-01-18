#include <iostream>
#include <assert.h>
#include <gurobi_c++.h>
#include <float.h>
#include <math.h>
#include <map>
#include <set>
#include <lemon/list_graph.h>
#include <lemon/unionfind.h>
#include <lemon/gomory_hu.h>
#include <lemon/adaptors.h>
#include <lemon/connectivity.h>
#include <lemon/dijkstra.h>
#include <unordered_map>
#include <fstream>
using namespace lemon;
using namespace std;
typedef ListDigraph Digraph;
typedef Digraph::ArcMap<double> ArcValueMap;
typedef Digraph::NodeMap<string> DNodeStringMap;
/*
                  | map             | unordered_map
---------------------------------------------------------
Ordering        | increasing  order   | no ordering
                | (by default)        |

Implementation  | Self balancing BST  | Hash Table
                | like Red-Black Tree |

search time     | log(n)              | O(1) -> Average
                |                     | O(n) -> Worst Case

Insertion time  | log(n) + Rebalance  | Same as search

Deletion time   | log(n) + Rebalance  | Same as search
*/

// typedef std::map< std::string, std::map<std::string, double> > Matrix;
typedef std::unordered_map<std::string, std::unordered_map<std::string, double>> Matrix;

class Experiment_Data
{
public:
    Experiment_Data();
    Experiment_Data(Digraph &dig,
                    DNodeStringMap &nodename,
                    ArcValueMap &exec_cost);

    Digraph &dg;
    int nnodes, narcs;
    DNodeStringMap &vname;
    ArcValueMap &cost;
    static void read_xls(string filename, Digraph &graph)
    {
        cout << "Reading" << endl;
        // return graph;
    }
};
std::vector<std::string> tokenize(const std::string &s, char c)
{
    auto end = s.cend();
    auto start = end;

    std::vector<std::string> v;
    for (auto it = s.cbegin(); it != end; ++it)
    {
        if (*it != c)
        {
            if (start == end)
                start = it;
            continue;
        }
        if (start != end)
        {
            v.emplace_back(start, it);
            start = end;
        }
    }
    if (start != end)
        v.emplace_back(start, end);
    return v;
}

void read_file(string file_name, Matrix &mtz)
{
    cout << "Comming zoom" << endl;
    cout << file_name << endl;
    std::ifstream infile(file_name);
    std::string line;
    for (std::string line; getline(infile, line);)
    {
        auto splited_string = tokenize(line, ' ');
        mtz[splited_string[0]][splited_string[1]] = stod(splited_string[2]);
    }
}

Experiment_Data::Experiment_Data(Digraph &dig, DNodeStringMap &nodename, ArcValueMap &exec_cost)
    : dg(dig), vname(nodename), cost(exec_cost)
{
    nnodes = countNodes(this->dg);
    narcs = countArcs(this->dg);
}

int makespam(Matrix &mty)
{
    cout << "Process ....." << endl;
    Digraph dg;
    string file_name;
    // Experiment_Data dat;
    Experiment_Data::read_xls(file_name, dg);
    int seed = 1;
    srand48(seed);
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env);
    model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.getEnv().set(GRB_IntParam_Seed, seed);
    model.set(GRB_StringAttr_ModelName, "Makespam Scheduling"); // name to the problem
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);            // is a minimization problem
                                                                // Add one binary variable for each edge and also sets its cost in the objective function
    char name[1000];
    map<int, GRBVar> x;
    // Digraph::ArcMap<GRBVar> x(D.dg); // Variable to vehicle edges
    // Digraph::ArcMap<GRBVar> y(D.dg); // Variable to drone edges
    int totalVariables = 10;
    for (int i = 0; i < totalVariables; i++)
    {
        std::string s = std::to_string(i);
        sprintf(name, "x_%s", s.c_str());
        x[i] = model.addVar(0.0, 1.0, 5, GRB_BINARY, name);
    }
    model.update();
    model.optimize();
    return 1;
}
int main(int argc, char *argv[])
{
    cout << "Hello World" << endl;
    string filename;
    filename = argv[1];
    Matrix mtx;
    read_file(filename, mtx);
    cout << mtx.size() << endl;
    makespam(mtx);
    return 0;
}
