#include "multithread_hashmap.h"
#include <iostream>

struct IPAddr{
	unsigned int ip_src; 
	unsigned int ip_dst; 
	
	bool operator ==(const struct IPAddr & lhs){
		if(this->ip_src==lhs.ip_src && this->ip_dst==lhs.ip_dst){
			return true; 
		}else{
			return false; 
		}
	
	}
};
class IPAddr_hash{
public:
	unsigned int operator()(const unsigned int &index){
		return index; 
	}
};

static void iphash_update_cb(Node<int ,IPAddr*> &node,unsigned int &data ,bool &endloop ){
	node.value->ip_dst = 0;
	node.value->ip_src = 0; 
	endloop = true; 
}
static void iphash_traversal_cb(Node<int, IPAddr*> &node,unsigned int &data,bool &endloop){
	if(node.key==100){
		std::cout << node.value->ip_dst << std::endl; 
	}
	if((node.value->ip_dst!=(node.key)*10)||(node.value->ip_src != (node.key) * 20)){
		assert(0); 
	}

}
int main(int argc, char *argv[])
{
	unsigned int value = 0; 
	hash_map<int, struct IPAddr *, IPAddr_hash> ip_hashmap;
	for(int i=0; i<100000; i++){
		IPAddr * ipaddr = new IPAddr(); 
		ipaddr->ip_dst = i * 10; 
		ipaddr->ip_src = i * 20; 
		ip_hashmap.insert(i, ipaddr); 
	}
	for(int i=0; i<100000; i++){
		IPAddr *ipaddr = NULL; 
		ip_hashmap.find(i, ipaddr); 
		if((ipaddr->ip_dst!=i*10)||(ipaddr->ip_src!=i*20)){
			std::cout << "error !\n" << std::endl; 
			assert(0); 
		}
		//std::cout << "dst:"<<ipaddr->ip_dst << "   src:"<<ipaddr->ip_src << std::endl; 
	}
	value = 100; 
	
	ip_hashmap.traversal(value, iphash_traversal_cb); 
	ip_hashmap.update(100, value, iphash_update_cb);
	IPAddr *ipaddr = NULL;
	ip_hashmap.find(100, ipaddr);
	std::cout << ipaddr->ip_src << std::endl; 
	std::cout << ipaddr->ip_dst << std::endl;
	//ip_hashmap.printNodeNumberInBucket(); 



}