

#include <string>
#include <set>
#include <fstream>
#include <iostream>

#include "graphchi_basic_includes.hpp"

using namespace graphchi;


#define CLIQUE_OUT_FILE
#define CLIQUE_DEBUG

#ifdef CLIQUE_DEBUG
std::ofstream dfile;
#endif

/**
  * Type definitions. Remember to create suitable graph shards using the
  * Sharder-program. 
  */

typedef std::set<vid_t> vlist;

// @task_t represents a indenpendent task to compute cliques from @cand
// @flag is the biggest index of vertices having visited
struct task_t{

    task_t( vlist *x_cand, vlist *x_c, vid_t x_flag,
            task_t *x_pre = NULL, task_t *x_next = NULL): 
        cand(x_cand), c(x_c), flag(x_flag), pre(x_pre), next(x_next){}

    vlist   *cand;
    vlist   *c;
    vid_t   flag;
    task_t  *pre;
    task_t  *next;
};

struct tasklist {

    tasklist(task_t *h, task_t *t): head(h), tail(t), len(0){}

    void remove_tail() {
        
        --len;

        if(tail == NULL && head == NULL){
            return ;
        } else if(tail != NULL && head != NULL && tail != head){
            tail->pre->next = NULL;
            tail = tail->pre;
        } else if(tail != NULL && head != NULL && tail == head){
            head = NULL;
            tail = NULL;
        } else {
            std::cout << "Remove Error" << std::endl;
            exit(0);
        }

    }

    void remove_head() {
        
        if(tail == NULL && head == NULL){
            return ;
        } else if (tail != NULL && head != NULL && tail != head) {
            head->next->pre = NULL;
            head = head->next;
        } else if (tail != NULL && head != NULL && tail == head) {
            head = NULL;
            tail = NULL;
        } else {
            std::cout << "Remove queue's head Error" << std::endl;
            exit(0);
        }
    }

    void insert_tail( task_t *tmp ) {
        
        ++len;

        if( head == NULL && tail == NULL ){
            head = tmp;
            tail = tmp;
            tmp->pre = NULL;
            tmp->next = NULL;
        } else if(head != NULL && tail != NULL){
            tmp->pre = tail;
            tail->next = tmp;
            tail = tmp;
            tmp->next = NULL;
        } else {
            std::cout << "Queue insert Error " << std::endl;
            exit(0);
        }

    }

    task_t *head;
    task_t *tail;
    size_t len;
};


vlist* 
get_intsct(vlist *v1, vlist *v2){
    
    vlist *res    = new vlist();
    vlist *lo     = v1->size() > v2->size() ? v2 : v1;
    vlist *check  = lo == v1 ? v2 : v1;

    for( vlist::const_iterator iter = lo->begin();
         iter != lo->end();
         ++iter )
        if( check->find(*iter) != check->end() )
            res->insert(*iter);

    return res;
}

void
release_task(task_t *t){
   
    delete t->cand;
    delete t->c;
    delete t;
}

vlist*
set_insert_copy( vlist* vl, vid_t v){
    
    vlist *res = new vlist(*vl);
    res->insert(v);
    return res;
}

vlist*
set_insert_copy( vid_t v ){
    
    vlist *res = new vlist();
    res->insert(v);
    return res;
}

/* this function only for debugging */
void
print_vlist(vlist *v){
   
    std::cout << "clique: ";
    for(vlist::iterator iter = v->begin();
        iter != v->end();
        ++iter )
        std::cout << *iter << " , ";
    std::cout << std::endl;
}

#ifdef CLIQUE_OUT_FILE
    std::ofstream cfile;
#endif
/* this function need to be finished */
void
write_clique_file( vlist* clique, std::ofstream &cfile){

    print_vlist(clique);
    for( vlist::iterator iter = clique->begin();
         iter != clique->end();
         ++iter) {
        
        cfile << *iter << " ";
    }
    cfile << std::endl;
}

typedef tasklist VertexDataType ;
typedef vlist* EdgeDataType;

/**
  * GraphChi programs need to subclass GraphChiProgram<vertex-type, edge-type> 
  * class. The main logic is usually in the update function.
  */
struct MyGraphChiProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {
    
 
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &vertex, graphchi_context &gcontext) {

        // Initialize the task queue of every @vertex
        // Construct a new task that contains all @vertex's neighbors
        // the insert it into task queue.
        if (gcontext.iteration == 0) {
            
            //tasklist *tmp_tlist = new tasklist(NULL, NULL);
            tasklist cur_tlist(NULL, NULL);
            vlist *cur_cand = new vlist();
            vlist *cur_c    = new vlist();
            for( int i = 0; i != vertex.num_edges(); ++i)
                cur_cand->insert(vertex.edge(i)->vertex_id());
            cur_c->insert(vertex.id());

            task_t *first_task = new task_t( cur_cand, cur_c, vertex.id() );
            cur_tlist.insert_tail(first_task);
            vertex.set_data(cur_tlist);

            // if @vertex.id() > neighbor.id()
            // @vertex should store its adjacency list in the edge between them
            for( int i = 0; i != vertex.num_edges(); ++i) {
                
                if( vertex.edge(i)->vertex_id() < vertex.id() ) {
                    vlist *e_val = new vlist(*cur_cand);
                    vertex.edge(i)->set_data(e_val);
                }
            }

        } else {
            
            tasklist *tasks = &(vertex.get_data());
            task_t   *t     = vertex.get_data().head;

            if ( t == NULL ){
                return ;
            }

            tasks->remove_head();
            if ( t->cand->size() == 0) {
                /* do something about storing maximal clique in t->c */
                #ifdef CLIQUE_OUT_FILE
                write_clique_file(t->c, cfile);
                #endif
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
                    vlist *candidate = get_intsct(adjlist, t->cand);
                    vlist *newc      = set_insert_copy(t->c, *iter);

                    if (candidate->size() != 0) {
                        task_t *tmp = new task_t(candidate, newc, *iter);
                        tasks->insert_tail(tmp);
                    } else {
                        // output clique
                        #ifdef CLIQUE_OUT_FILE
                        write_clique_file(newc, cfile);
                        #endif
                        delete candidate;
                        delete newc;
                    }
                } // for end
            }

            release_task(t);


        } // else end // for iteration != 0
    }
    
    void before_iteration(int iteration, graphchi_context &gcontext) {
    }
    
    void after_iteration(int iteration, graphchi_context &gcontext) {
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
    metrics m("my-application-name");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file");  // Base filename
    int niters           = get_option_int("niters", 4); // Number of iterations
    bool scheduler       = get_option_int("scheduler", 0); // Whether to use selective scheduling

#ifdef CLIQUE_OUT_FILE
    cfile.open("AllClique.data");
#endif

#ifdef CLIQUE_DEBUG
    dfile.open("debug.log");
#endif

    /* Detect the number of shards or preprocess an input to create them */
    int nshards          = convert_if_notexists<EdgeDataType>(filename, 
                                                            get_option_string("nshards", "auto"));
    
    /* Run */
    MyGraphChiProgram program;
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m); 
    engine.run(program, niters);
    
    /* Report execution metrics */
    metrics_report(m);
#ifdef CLIQUE_OUT_FILE
    cfile.close();
#endif

#ifdef CLIQUE_DEBUG
    dfile.close();
#endif

    return 0;
}


