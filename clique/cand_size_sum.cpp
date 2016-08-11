
#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include "graphchi_basic_includes.hpp"
#include "clique_bfs.hpp"

using namespace graphchi;

//#define CLIQUE_OUT_FILE

bool converged = true;

#ifdef CLIQUE_OUT_FILE
    std::ofstream cfile;
#endif

/* Global variables definitions */
uint32_t total_cand_size;

typedef tasklist VertexDataType ;
typedef vlist* EdgeDataType;

struct ComputeCandSum: public GraphChiProgram<VertexDataType, EdgeDataType> {
    
 
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &vertex, graphchi_context &gcontext) {

        // Initialize the task queue of every @vertex
        // Construct a new task that contains all @vertex's neighbors
        // then insert it into task queue.
        if (gcontext.iteration == 0) {
            /* @cur_cand and @cur_c is used for the first task of @vertex */
            vlist *cur_cand = new vlist();

            for( int i = 0; i != vertex.num_edges(); ++i){
                
                if( vertex.edge(i)->vertex_id() > vertex.id() )
                    cur_cand->insert(vertex.edge(i)->vertex_id());
            }

            total_cand_size += cur_cand->size();


        } else {
            /* In this application, we have no works to do here */
        } 
    }
    
    void before_iteration(int iteration, graphchi_context &gcontext) {
        converged = true;
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
    metrics m("Candidate Set Size Sum Compute");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file");  // Base filename
    int niters           = get_option_int("niters", 2); // Number of iterations
    bool scheduler       = get_option_int("scheduler", 0); // Whether to use selective scheduling


    /* global variables initialization */ 
    total_cand_size = 0;

    /* Detect the number of shards or preprocess an input to create them */
    int nshards          = convert_if_notexists<EdgeDataType>(filename, 
                                                            get_option_string("nshards", "auto"));
    
    /* Run */
    ComputeCandSum program;
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m); 
    engine.set_enable_deterministic_parallelism(false);
    engine.run(program, niters);
    
    /* Report execution metrics */
    metrics_report(m);

    /* Personal statics report */
    for(int i = 0; i != 40; i++)
        std::cout << "*";
    std::cout << " Personal Report ";
    for(int i = 0; i != 40; i++)
        std::cout << "*";
    std::cout << std::endl;

    std::cout << "Total Cand Sets' Sum: " << total_cand_size << std::endl;

    return 0;
}


