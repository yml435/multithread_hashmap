#ifndef  _MULTITHREAD_BUCKET_H
#define  _MULTITHREAD_BUCKET_H

#include "multiThread_node.h"
#include "multiThread_pool.h"
#include <pthread.h>
#include <iostream>
#include <assert.h>
#include <error.h>

template<typename _Key ,typename _Value,typename _KeyEqual>
class Bucket{
public:
	Bucket(){
		bucket_size = 0;
		signature = 0; 
		node_num = 0; 
		bucket_head = NULL; 
		_rwlock = (pthread_rwlock_t*)malloc(sizeof(pthread_rwlock_t));//rwlock for buckets
		nxt = NULL; 
	}
	~Bucket(){
		struct Node<_Key, _Value> *node_p; 
		struct Node<_Key, _Value> *tmp = NULL; 
		pthread_rwlock_wrlock(_rwlock); 
		node_p = bucket_head;
		while(node_p!=NULL){
			tmp = node_p->nxt; 
			delete node_p; 
			node_p = tmp;
		}
		pthread_rwlock_unlock(_rwlock); 
	}
	bool put(unsigned int signature,const _Key &key,const _Value &value,Pool<_Key,_Value> *pool){
		pthread_rwlock_rdlock(_rwlock); 
		if(true==find_node(signature,key)){
			pthread_rwlock_unlock(_rwlock);
			return false;    //the node is already in the bucket
		}
		pthread_rwlock_unlock(_rwlock);
		struct Node<_Key, _Value> *node = pool->get_freenode();
		node->key = key; 
		node->value = value; 
		node->signature = signature; 
		pthread_rwlock_wrlock(_rwlock); 
		insertNode(node); 
		pthread_rwlock_unlock(_rwlock); 
		return true; 
	}
	bool find(unsigned int signature,const _Key &key,_Value &value){
		struct Node<_Key, _Value> *node = NULL;
		pthread_rwlock_rdlock(_rwlock); 
		node = bucket_head; 
		while(node!=NULL){
			if((signature==node->signature)&&m_equal_to(key,node->key)){
				value = node->value;
				pthread_rwlock_unlock(_rwlock); 
				return true; 
			}
			node = node->nxt; 
		}
		return false; 
		pthread_rwlock_unlock(_rwlock); 
	}
	template<typename _Params,typename _Modifier>
	void traversal(_Params &paras,_Modifier &action){
		if(NULL==bucket_head){
			return; 
		}
		struct Node<_Key, _Value> *node = NULL;
		bool endloop = false; 
		pthread_rwlock_wrlock(_rwlock); 
		node = bucket_head; 
		while(NULL!=node){
			if(true==endloop){
				break;
			}
			action(*node, paras, endloop);
			node = node->nxt; 
		}
		pthread_rwlock_unlock(_rwlock); 
	}
	template<typename _Params ,typename _Modifier>
	bool update(unsigned int signature,const _Key &key,_Params &paras,_Modifier &action){
		if(NULL==bucket_head){
			return false; 
		}
		struct Node<_Key, _Value> *node = NULL; 
		pthread_rwlock_wrlock(_rwlock); 
		node = get_node(signature,key);
		if(NULL==node){
			pthread_rwlock_unlock(_rwlock); 
			return false; 
		}
		bool endloop; 
		action(*node, paras,endloop); 
		pthread_rwlock_unlock(_rwlock); 
		return true; 
	}
	template<typename _Params, typename _Modifier>
	bool removebycb(_Params &paras, _Modifier &action,Pool<_Key,_Value> *pool){
		if(NULL==bucket_head){
			return false; 
		}
		struct Node<_Key, _Value> *node = NULL; 
		struct Node<_Key, _Value> *pre  = NULL; 
		struct Node<_Key, _Value> *free = NULL;
		
		bool endloop=false; 
		bool needremove = false; 
		pthread_rwlock_wrlock(_rwlock); 
		node = bucket_head; 
		pre = bucket_head; 
		while(node!=NULL){
			if(true==endloop){
				pthread_rwlock_unlock(_rwlock); 
				return false; 
			}
			action(*node, paras, needremove, endloop); 
			if(true==needremove){
				struct Node<_Key, _Value> *tmp = pre; 
				free = node; 
				pre->nxt = node->nxt; 
				node = node->nxt; 
				pre = tmp; 
				free->nxt = NULL; 
#ifdef DEBUG
				node_num--; 
#endif // DEBUG

				pool->put_freenode(free); 
			}else{
				pre = node;
				node = node->nxt;
			}
			needremove = false; 
		}
		pthread_rwlock_unlock(_rwlock); 
		return true;
	}
	bool remove(unsigned int signature,const _Key &key,Pool<_Key,_Value> *pool){
		if(bucket_head==NULL){
			return false; 
		}
		struct Node<_Key, _Value> *pre = NULL;
		struct Node<_Key, _Value> *node = NULL;
		pthread_rwlock_wrlock(_rwlock); 
		node = bucket_head; 
		pre = bucket_head;
		bool needremove = false; 
		while (node != NULL ){
			if(equal_to(node->key,key)&&(key->signature== signature)){
				needremove = true; 
			}
			if(true==needremove){
				pre->nxt = node->nxt; 
				node->nxt = NULL; 
				pool->put_freenode(node); 
				pthread_rwlock_unlock(_rwlock); 
#ifdef DEBUG
				node_num--; 
#endif
				return true; 
			}
			pre = node; 
			node = node->nxt; 
		}
		pthread_rwlock_unlock(_rwlock); 
	}
	template<typename _Params, typename _Modifier>
	bool remove(unsigned int signature,const _Key &key,_Params &paras, _Modifier &action,Pool<_Key,_Value> *pool){
		if(bucket_head==NULL){
			return false; 
		}
		struct Node<_Key, _Value> *pre = NULL;
		struct Node<_Key, _Value> *node = NULL;
		struct Node<_Key, _Value> *free = NULL; 
		bool needremove = false;
		bool endloop = false; 
		pthread_rwlock_wrlock(_rwlock); 
		node = bucket_head; 
		pre = node; 
		while(node!=NULL){
			if(true==endloop){
				pthread_rwlock_unlock(_rwlock); 
				return false; 
			}
			if(equal_to(key,node->key)&&(signature==node->signature)){
				action(*node, paras, needremove, endloop); 
			}
			if(needremove==true){
				struct Node<_Key, _Value> *tmp = pre;
				free = node;
				pre->nxt = node->nxt; 
				node = node->nxt; 
				pre = tmp; 
				free->nxt = NULL; 
				pool->put_freenode(free);
				pthread_rwlock_unlock(_rwlock);
#ifdef DEBUG
				node_num--; 
#endif // DEBUG

				return true; 
			}else{
				pre = node; 
				node = node->nxt; 
			}			
		}
		pthread_rwlock_unlock(_rwlock); 
		return false; 
	}

public:
	Bucket *nxt; 

#ifdef  DEBUG 
	unsigned int node_num; 
#endif //  DEBUG 


private:
	unsigned int bucket_size;//the size of this bucket
	struct Node<_Key,_Value>  *bucket_head; 
	pthread_rwlock_t *_rwlock; 
	_KeyEqual m_equal_to; 
	unsigned int signature; 
	bool find_node(unsigned int signature,const _Key &key){
		struct Node<_Key, _Value>  *tmp = bucket_head; 
		while(NULL!=tmp){
			if(m_equal_to(key,tmp->key)&&(signature==tmp->signature)){
				return true; 
			}
			tmp = tmp->nxt; 
		}
		return false; 
	}
	void insertNode(struct Node<_Key,_Value> *node){
		node->nxt = bucket_head; 
		bucket_head = node; 
#ifdef  DEBUG
		node_num++; 
#endif //  DEBUG


	}
	struct Node<_Key,_Value> *get_node(unsigned int signature,const _Key & key){
		struct Node<_Key, _Value> *tmp = bucket_head; 
		while(NULL!=tmp){
			if(m_equal_to(key,tmp->key)&&(tmp->signature==signature)){
				return tmp; 
			}
			tmp = tmp->nxt; 
		}
		return NULL; 
	}
};

#endif 

