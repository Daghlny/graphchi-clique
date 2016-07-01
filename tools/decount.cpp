/**
 * @file
 * @author  Aapo Kyrola <akyrola@cs.cmu.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright [2012] [Aapo Kyrola, Guy Blelloch, Carlos Guestrin / Carnegie Mellon University]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 
 *
 * @section DESCRIPTION
 *
 * Simple pagerank implementation. Uses the basic vertex-based API for
 * demonstration purposes. A faster implementation uses the functional API,
 * "pagerank_functional".
 */

#include <string>
#include <fstream>
#include <cmath>

#define GRAPHCHI_DISABLE_COMPRESSION


#include "graphchi_basic_includes.hpp"
#include "util/toplist.hpp"

using namespace graphchi;
 
#define THRESHOLD 1e-1    
#define RANDOMRESETPROB 0.15


typedef vid_t VertexDataType;
typedef vid_t EdgeDataType;

struct PagerankProgram : public GraphChiProgram<VertexDataType, EdgeDataType> {
    
    /**
      * Called before an iteration starts. Not implemented.
      */
    void before_iteration(int iteration, graphchi_context &info) {
		std::cout << "test_before" << std::endl;     
    }
    
    /**
      * Called after an iteration has finished. Not implemented.
      */
    void after_iteration(int iteration, graphchi_context &ginfo) {
		std::cout << "test_after" << std::endl;
    }
    
    /**
      * Called before an execution interval is started. Not implemented.
      */
    void before_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &ginfo) {        
    }
    
    
    /**
      * Pagerank update function.
      */
    void update(graphchi_vertex<VertexDataType, EdgeDataType> &v, graphchi_context &ginfo) {
	            v.set_data(v.num_edges()); 
        
    }
    
};

int main(int argc, const char ** argv) {
    graphchi_init(argc, argv);
    metrics m("pagerank");
    global_logger().set_log_level(LOG_DEBUG);
    std::cout << "Begin!" << std::endl;
    /* Parameters */
    std::string filename    = get_option_string("file"); // Base filename
    int niters              = get_option_int("niters", 4);
    bool scheduler          = false;                    // Non-dynamic version of pagerank.
    int ntop                = get_option_int("top", 20);
    
    /* Process input file - if not already preprocessed */
    int nshards             = convert_if_notexists<EdgeDataType>(filename, get_option_string("nshards", "auto"));

    /* Run */
    graphchi_engine<vid_t, vid_t> engine(filename, nshards, scheduler, m); 
    engine.set_modifies_inedges(false); // Improves I/O performance.
    PagerankProgram program;
    engine.run(program, niters);

    
    /* Output top ranked vertices */
    std::vector< vertex_value<vid_t> > top = get_top_vertices<vid_t>(filename, ntop);
    std::cout << "Print top " << ntop << " vertices:" << std::endl;
    for(int i=0; i < (int)top.size(); i++) {
       std::cout << (i+1) << ". " << top[i].vertex << "\t" << top[i].value << std::endl;
	//	logstream(LOG_INFO) << i+1 << top[i].vertex << "\t" << top[i].value << std::endl;
    }
    
    metrics_report(m);    
    return 0;
}

