
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
#include <stack>
#include <iostream>
#include "graphchi_basic_includes.hpp"

using namespace graphchi;

#define GRAPHCHI_DISABLE_COMPRESSION

/**
  * Type definitions. Remember to create suitable graph shards using the
  * Sharder-program. 
  */

struct bk_set{

    bk_set(size_t c_len, size_t cand_len, size_t nc_len):
        c(c_len), cand(cand_len), nc(nc_len){}
    bk_set(std::set<vid_t> xc, std::set<vid_t> xcand, std::set<vid_t> xnc):
        c(xc), cand(xcand), nc(xnc){}

    std::set<vid_t> c;
    std::set<vid_t> cand;
    std::set<vid_t> nc;
}

typedef std::stack<bk_set> bk_stack;

struct bk_node{

    bk_stack *stack;
    bk_set   *curr;
}


/*
struct bk_tree_t {

    vid_t *c;
    vid_t *cand;
    size_t ne;
    size_t len;
};
*/


typedef struct bk_node VertexDataType;
typedef std::set<vid_t>*  EdgeDataType;

/**
  * GraphChi programs need to subclass GraphChiProgram<vertex-type, edge-type> 
  * class. The main logic is usually in the update function.
  */
struct MyGraphChiProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {
    
 
    /**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &vertex, graphchi_context &gcontext) {

        if (gcontext.iteration == 0) {
           
            // on first iteration, build every vertex's search tree root
            
            bk_node tmp_value;
            tmp_value.stack = new bk_stack(0); 
            tmp_value.curr  = NULL;
            vertex.set_data(tmp_value);
            
            // construct a set storing all neighbors of @vertex
            std::set<vid_t> all_neighbors();
            for( int i = 0; i != vertex.num_edges(); ++i )
                all_neighbors.insert( vertex.edge(i)->vertex_id() );

            // when a neighbor's id is smaller than @vertex,
            // this neighbor should get all the neighbors of @vertex
            for( int i = 0; i != vertex.num_edges(); ++i )
                if( vertex.edge(i)->vertex_id() < v.id() ){
                    std::set<vid_t> *tmp_edge = 
                        new std::set<vid_t>(all_neighbors);
                    vertex.edge(i)->set_data(tmp_edge);
                }

        } else {
            /* Loop over all edges (ignore direction) */
            // 

            bk_node node = vertex.get_data();
            if( node->curr->cand.size() == 0 
                && node->curr->nc.size() == 0){

                output_clique(node->curr->c);
                return;
            }

            // create every candidate branch of current search subtree node
            for( std::set<vid_t>::iterator iter = node->curr->cand.begin(); 
                 iter != node->curr->cand.end(); 
                 ++iter ){

                tmp_vid = *iter;
                node->curr->c.insert(tmp_vid);
                node->curr->cand()
                bk_set new_set(node->curr->c)
            }
            
            for(int i=0; i < vertex.num_edges(); i++) {
                
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

