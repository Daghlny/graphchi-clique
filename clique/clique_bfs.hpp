
#ifndef __GRAPHCHI__CLIQUE__
#define __GRAPHCHI__CLIQUE__

#include <set>
#include <iostream>

#include "graphchi_basic_includes.hpp"

using namespace graphchi;

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

        --len;
        
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

#endif

