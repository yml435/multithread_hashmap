#ifndef _MULTITHREAD_NODE_H
#define _MULTITHREAD_NODE_H


template<typename _Key,typename _Value>
struct  Node{
public:
	_Key key; 
	_Value value;

	unsigned int signature; 
	struct Node *nxt;
	Node(const _Key &_key,const _Value &_value):key(_key),value(_value){
		this->nxt = NULL; 
		signature = 0; 
	}
	Node(){
		this->nxt = NULL;
		signature = 0;
	}
	template<typename _Params,typename _Modifer>
	void node_cb(_Params &paras,_Modifer &action,bool &endloop){
		action(*this, paras, endloop); 
	}
};

#endif 
