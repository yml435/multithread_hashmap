#ifndef _MULTITHREAD_POOL_H
#define _MULTITHREAD_POOL_H

#include "multiThread_node.h"
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#define MAX_POOL_SIZE	1024*4

template<typename _Key, typename _Value>
class Pool{
public:
	Pool(){
		max_poolsize = MAX_POOL_SIZE; 	
		init_pool(max_poolsize); 
		_pool_rwlock= (pthread_rwlock_t*)malloc(sizeof(pthread_rwlock_t));
	}
	Pool(unsigned int max_size) :max_poolsize(max_size){
		init_pool(max_poolsize); 
		_pool_rwlock = (pthread_rwlock_t*)malloc(sizeof(pthread_rwlock_t));
	}
	struct Node<_Key,_Value> *get_freenode(){
		struct Node<_Key, _Value> *node = NULL; 
		pthread_rwlock_wrlock(_pool_rwlock); 
		if(idle_nodes==0&&idle_nodes_head==NULL){
			node = new Node<_Key, _Value>(); 
			this->max_poolsize++;
			pthread_rwlock_unlock(_pool_rwlock); 
			return  node; 
		}
		pthread_rwlock_wrlock(_pool_rwlock); 
		node = idle_nodes_head;
		idle_nodes_head = idle_nodes_head->nxt;
		this->idle_nodes--; 
		pthread_rwlock_unlock(_pool_rwlock); 
		return node; 
	}
	void put_freenode(struct Node<_Key,_Value> *node){
		if(NULL==node){
			return; 
		}
		pthread_rwlock_wrlock(_pool_rwlock); 
		node->nxt = idle_nodes_head; 
		idle_nodes_head->nxt = node; 
		this->idle_nodes++; 
		pthread_rwlock_unlock(_pool_rwlock); 
	}
	~Pool(){
		struct Node<_Key, _Value> *tmp = NULL; 
		pthread_rwlock_wrlock(_pool_rwlock); 
		struct Node<_Key, _Value> *node = idle_nodes_head; 
		while(node!=NULL){
			tmp = node->nxt; 
			delete node; 
			this->idle_nodes--; 
			node = node->nxt; 
		}
		pthread_rwlock_unlock(_pool_rwlock); 
	}



private:
	unsigned int max_poolsize; 
	unsigned int idle_nodes;
	pthread_rwlock_t *_pool_rwlock; 
	struct Node<_Key, _Value> *idle_nodes_head; 
	
	void init_pool(unsigned int size){
		struct Node<_Key, _Value> **node = &idle_nodes_head;
		for(int count=0; count<size; count++){
			*node = new Node<_Key, _Value>();
			if(NULL==*node){
				assert(0); 
			}
			*node = (*node)->nxt; 
		}
	}
};
#endif 