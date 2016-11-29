#ifndef _MULTITHREAD_COMMON_H
#define _MULTITHREAD_COMMON_H
#include <stddef.h>
/*
	This is the hash function for hash_map or hash_multimap
*/
template<typename _Key>
struct hash{
	unsigned int  operator()(const _Key & key) const{
		return 0; 
	}
};

#endif 
