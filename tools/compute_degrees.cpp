
#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include "graphchi_basic_includes.hpp"
#include "clique_bfs.hpp"

using namespace graphchi;

typedef tasklist VertexDataType ;
typedef vlist* EdgeDataType;

struct CliqueGraphChiProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {
    
 
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &vertex, graphchi_context &gcontext) {

        if( gcontext.iteration == 0 ) 
            

    }
    
    void before_iteration(int iteration, graphchi_context &gcontext) {
        converged = true;
        curr_iteration_task_num = 0;
    }
    
    void after_iteration(int iteration, graphchi_context &gcontext) {
        /* output remaining task's number */
        //std::cout << curr_iteration_task_num << std::endl;
        if(converged && iteration != 0){
            gcontext.set_last_iteration(iteration);
        }
    }
    
    void before_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {        
    }
    
    void after_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {        
    }
    
};

int main(int argc, const char ** argv) {
    /* GraphChi initialization will read the command line 
       arguments and the configuration file. */
    graphchi_init(argc, argv);
    
    /* Metrics object for keeping track of performance counters
       and other information. Currently required. */
    metrics m("Maximal Cliques Enumeration");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file");  // Base filename
    int niters           = get_option_int("niters", 1000000000); // Number of iterations
    bool scheduler       = get_option_int("scheduler", 0); // Whether to use selective scheduling

    /* global variables init */ 
    task_per_iter        = get_option_int("taskPerIter", 10);// get the task's number of each iteration
    max_clique_size = 0;
    clique_num = 0;
    max_cand_size = 0;
#ifdef CLIQUE_OUT_FILE
    std::string clique_filename = filename+".graphchi.clique";
    cfile.open(clique_filename.c_str());
#endif

    /* Detect the number of shards or preprocess an input to create them */
    int nshards          = convert_if_notexists<EdgeDataType>(filename, 
                                                            get_option_string("nshards", "auto"));
    
    /* Run */
    CliqueGraphChiProgram program;
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m); 
    engine.set_enable_deterministic_parallelism(false);
    engine.run(program, niters);
    
    /* Report execution metrics */
    metrics_report(m);
    
    std::cout << "Total clique number: " << clique_num << std::endl;
    std::cout << "Maximum clique's size: " << max_clique_size << std::endl;
    std::cout << "Maximum Candidate's size: " << max_cand_size << std::endl;

#ifdef CLIQUE_OUT_FILE
    cfile.close();
#endif

    return 0;
}


