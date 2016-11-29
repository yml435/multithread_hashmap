#ifndef _MULTITHREAD_HASHMAP_H
#define _MULTITHREAD_HASHMAP_H
#include "multiThread_common.h"
#include "multithread_bucket.h"
#include "multiThread_pool.h"

#ifdef DEBUG
#include <stdio.h>
#endif 
#define  BUCKETS_NUM  1024

template <typename _Key, typename _Value, typename HashFun=hash<_Key >, typename _KeyEqual = std::equal_to<_Key> >
class hash_map{

public:
	hash_map(){
		_pool = new Pool<_Key,_Value>(); 
		buckets_num = BUCKETS_NUM; 
		mask = buckets_num - 1;
		buckets_array = new Bucket<_Key,_Value, _KeyEqual>[buckets_num];
	}
	hash_map(unsigned int pool_size,unsigned int _buckets_num){
		_pool = new Pool<_Key, _Value>(pool_size);
		buckets_array = NULL; 
		buckets_num = _buckets_num; 
		mask = buckets_num - 1; 
		buckets_array = new Bucket<_Key, _Value, _KeyEqual>[buckets_num];
		
	}
	~hash_map(){
		delete _pool; 
		delete[] buckets_array; 
	}
	bool insert(const _Key &key,const _Value &value){
		unsigned int signature = _hash(key);
		Bucket<_Key, _Value, _KeyEqual> *bucket = get_bucket_byname(signature);
		return bucket->put(signature, key, value, _pool); 
	}
	bool remove(const _Key &key){
		unsigned int signature = _hash(key); 
		Bucket<_Key, _Value, _KeyEqual> *bucket = get_bucket_byname(signature);
		return bucket->remove(signature, key,_pool); 
	}

	template<typename _Params , typename _Modifier>
	bool remove(const _Key &key,_Params &paras, _Modifier &action){
		unsigned int signature = _hash(key);
		Bucket<_Key, _Value, _KeyEqual> *bucket = get_bucket_byname(signature);
		return bucket->remove(signature, key, paras, action, _pool); 
	}

	template<typename _Params, typename _Modifier>
	void removebycb(_Params &paras,_Modifier &action){
		for(int index=0; index<buckets_num; index++){
			(&buckets_array[index])->removebycb(paras, action, _pool); 
		}
	}
	template<typename _Params, typename _Modifier>
	bool update(const _Key &key,_Params &paras,_Modifier &action){
		unsigned int signature = _hash(key);
		Bucket<_Key, _Value, _KeyEqual> *bucket = get_bucket_byname(signature);
		return bucket->update(signature, key, paras, action); 
	}

	template<typename _Params, typename _Modifier>
	void traversal(_Params &paras,_Modifier &action){
		for(int i=0; i<buckets_num; i++){
			(&buckets_array[i])->traversal(paras, action); 
		}
	}
	bool find(const _Key &key, _Value &value){
		unsigned int signature = _hash(key);
		Bucket<_Key, _Value, _KeyEqual> *bucket = get_bucket_byname(signature);
		return bucket->find(signature, key, value); 
	}
#ifdef DEBUG
	void printNodeNumberInBucket(void){
		for(int i=0; i<buckets_num; i++){
			printf("nodes in buckets %d\n", buckets_array[i].node_num); 
		}
	}
#endif

private:
	HashFun _hash;
	Bucket<_Key,_Value,_KeyEqual> *buckets_array;
	Pool<_Key, _Value>  *_pool;
	unsigned int buckets_num; 
	unsigned int mask ; 

	Bucket<_Key, _Value,_KeyEqual> *get_bucket_byname(unsigned int signature){
		unsigned int index = signature & mask; 
		return &(buckets_array[index]); 
	}
};

#endif 