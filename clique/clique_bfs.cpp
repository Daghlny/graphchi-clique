
#include <string>
#include <set>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#include "graphchi_basic_includes.hpp"
#include "clique_bfs.hpp"

using namespace graphchi;

#define CLIQUE_OUT_FILE
//#define CLIQUE_DEBUG

#ifdef CLIQUE_DEBUG
std::ofstream dfile;
#endif

uint64_t  curr_iteration_task_num;
uint64_t  max_clique_size;
uint64_t  clique_num;
int       task_per_iter;

/* compute the intersection of two sets */
vlist* 
get_intsct(vlist *v1, vlist *v2){
    
    vlist *res    = new vlist();
    /* new version of intersect operation, time complexity is O(n+m) */
    vlist::iterator i = v1->begin(), j = v2->begin();
    while( i != v1->end() && j != v2->end() ) {
        if(*i < *j) i++;
        else if (*j < *i) j++;
        else {
            res->insert(*i);
            i++;
            j++;
        }
    }
    return res;
}

/* release the memory of a task */
void
release_task(task_t *t){
   
    delete t->cand;
    delete t->c;
    delete t;
}

/* return a new set containing the set @vl and the vertex @v */
vlist*
set_insert_copy( vlist* vl, vid_t v){
    
    vlist *res = new vlist(*vl);
    res->insert(v);
    return res;
}

/* return a new set only containing a vertex @v */
vlist*
set_insert_copy( vid_t v ){
    
    vlist *res = new vlist();
    res->insert(v);
    return res;
}

/* this function only for debugging */
void
print_vlist(vlist *v){
   
    std::cout << "vlist: ";
    for(vlist::iterator iter = v->begin();
        iter != v->end();
        ++iter )
        std::cout << *iter << " , ";
    std::cout << std::endl;
}

/* write the clique's vertex into the file @cfile */
void
write_clique_file( vlist* clique, std::ofstream &cfile){

    for( vlist::iterator iter = clique->begin();
         iter != clique->end();
         ++iter) {
        
        cfile << *iter << " ";
    }
    cfile << std::endl;
}

typedef tasklist VertexDataType ;
typedef vlist* EdgeDataType;

bool converged = true;

#ifdef CLIQUE_OUT_FILE
    std::ofstream cfile;
#endif

struct CliqueGraphChiProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {
    
 
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &vertex, graphchi_context &gcontext) {

        // Initialize the task queue of every @vertex
        // Construct a new task that contains all @vertex's neighbors
        // then insert it into task queue.
        if (gcontext.iteration == 0) {
            
            tasklist cur_tlist(NULL, NULL);
            vlist *cur_cand = new vlist();
            vlist *cur_c    = new vlist();

            vlist *all_neibors = new vlist();

            for( int i = 0; i != vertex.num_edges(); ++i){
                
                cur_cand->insert(vertex.edge(i)->vertex_id());
                all_neibors->insert(vertex.edge(i)->vertex_id());
            }

            cur_c->insert(vertex.id());

            task_t *first_task = new task_t( cur_cand, cur_c, vertex.id() );
            cur_tlist.insert_tail(first_task);
            vertex.set_data(cur_tlist);

            // if @vertex.id() > neighbor.id()
            // @vertex should store its adjacency list in the edge between them
            for( int i = 0; i != vertex.num_edges(); ++i) {
                
                if( vertex.edge(i)->vertex_id() < vertex.id() ) {
                    vertex.edge(i)->set_data(all_neibors);
                }
            }

            max_clique_size = 0;
            clique_num = 0;

        } else {
            
            // In this iteration, every vertex should process @task_per_iter tasks;
            for(int i = 0; i != task_per_iter; ++i){

                tasklist *tasks = vertex.get_data_ptr();
                task_t   *t     = tasks->head;

                if ( t == NULL ){
                    return ;
                }

                converged = false;

                tasks->remove_head();
                if ( t->cand->size() == 0) {
                    if( t->c->size() != 0 ){
                        clique_num++;
                        max_clique_size = std::max(max_clique_size, t->c->size());

                        /* output maximal clique in t->c */
                        #ifdef CLIQUE_OUT_FILE
                        //write_clique_file(t->c, cfile);
                        #endif
                    }
                    release_task(t);
                    return ;
                }

                if ( t->cand->size() != 0 ) {
                    
                    for( vlist::iterator iter = t->cand->begin();
                         iter != t->cand->end();
                         ++iter) {
                        
                        if ( *iter < t->flag ) continue;

                        /* Add New Task */
                        // First, construct two necessary vertex sets
                        // Second, if candidate set is empty, c is a MC
                        vlist *adjlist = NULL;
                        for( int i = 0; i != vertex.num_edges(); ++i ){
                            if (vertex.edge(i)->vertex_id() == *iter){ 
                                adjlist = vertex.edge(i)->get_data();
                                break;
                            }
                        }

                        if( adjlist == NULL ) {
                            std::cout << "Runtime Error: "; 
                            std::cout << "in current task, adjlist is NULL \n";
                            std::cout << "current vertex: " << vertex.id() ;
                            std::cout << std::endl;
                            std::cout << "cand: ";
                            print_vlist(t->cand);
                            std::cout << "c: ";
                            print_vlist(t->c);
                            std::cout << "flag: " << t->flag << std::endl;
                            exit(0);
                        }

                        vlist *candidate = get_intsct(adjlist, t->cand);
                        vlist *c         = set_insert_copy(t->c, *iter);

                        if (candidate->size() != 0) {
                            task_t *tmp = new task_t(candidate, c, *iter);
                            tasks->insert_tail(tmp);
                        } else {
                            max_clique_size = std::max(max_clique_size, c->size());
                            clique_num++;
                            // output clique
                            #ifdef CLIQUE_OUT_FILE
                            //write_clique_file(c, cfile);
                            #endif
                            delete candidate;
                            delete c;
                        }
                    } // for end
                }

                release_task(t);
            }

        } // else end // for iteration != 0
        curr_iteration_task_num += vertex.get_data_ptr()->len;
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
    task_per_iter        = get_option_int("taskPerIter", 10);// get the task's number of each iteration

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

#ifdef CLIQUE_OUT_FILE
    cfile.close();
#endif

    return 0;
}


