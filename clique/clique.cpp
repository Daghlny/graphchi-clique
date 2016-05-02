
/**
 * @file    clique.cpp
 * @author  Yinuo Li < liyinuo@hust.edu.cn >
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 */



#include <string>
#include <set>
#include "graphchi_basic_includes.hpp"

using namespace graphchi;

/**
  * Type definitions. Remember to create suitable graph shards using the
  * Sharder-program. 
  */

struct bk_tree_t {

    vid_t *c;
    size_t c_index;
    size_t cand_index;
    size_t len;
}

struct nb_info_t{
    vid_t *nbs;
    size_t len;
}

typedef struct bk_tree_t VertexDataType;
typedef struct nb_info_t EdgeDataType;

/**
  * GraphChi programs need to subclass GraphChiProgram<vertex-type, edge-type> 
  * class. The main logic is usually in the update function.
  */
struct MyGraphChiProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {
    
 
    /**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &vertex, graphchi_context &gcontext) {

        if (ginfo.iteration == 0) {
            /* On first iteration, initialize vertex (and its edges). This is usually required, because
               on each run, GraphChi will modify the data files. To start from scratch, it is easiest
               do initialize the program in code. Alternatively, you can keep a copy of initial data files. */
            // vertex.set_data(init_value);
            
        } else {
            /* Do computation */ 

            /* Loop over in-edges (example) */
            for(int i=0; i < vertex.num_inedges(); i++) {
                // Do something
            //    value += vertex.inedge(i).get_data();
            }
            
            /* Loop over out-edges (example) */
            for(int i=0; i < vertex.num_outedges(); i++) {
                // Do something
                // vertex.outedge(i).set_data(x)
            }
            
            /* Loop over all edges (ignore direction) */
            for(int i=0; i < vertex.num_edges(); i++) {
                // vertex.edge(i).get_data() 
            }
            
            // v.set_data(new_value);
        }
    }
    
    /**
     * Called before an iteration starts.
     */
    void before_iteration(int iteration, graphchi_context &gcontext) {
    }
    
    /**
     * Called after an iteration has finished.
     */
    void after_iteration(int iteration, graphchi_context &gcontext) {
    }
    
    /**
     * Called before an execution interval is started.
     */
    void before_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {        
    }
    
    /**
     * Called after an execution interval has finished.
     */
    void after_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {        
    }
    
};

int main(int argc, const char ** argv) {
    /* GraphChi initialization will read the command line 
       arguments and the configuration file. */
    graphchi_init(argc, argv);
    
    /* Metrics object for keeping track of performance counters
       and other information. Currently required. */
    metrics m("my-application-name");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file");  // Base filename
    int niters           = get_option_int("niters", 4); // Number of iterations
    bool scheduler       = get_option_int("scheduler", 0); // Whether to use selective scheduling
    
    /* Detect the number of shards or preprocess an input to create them */
    int nshards          = convert_if_notexists<EdgeDataType>(filename, 
                                                            get_option_string("nshards", "auto"));
    
    /* Run */
    MyGraphChiProgram program;
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m); 
    engine.run(program, niters);
    
    /* Report execution metrics */
    metrics_report(m);
    return 0;
}

