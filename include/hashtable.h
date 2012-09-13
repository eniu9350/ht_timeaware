#ifndef _HTTA_HASH_TABLE_H_
#define _HTTA_HASH_TABLE_H_

#include "common.h"



typedef struct _value{
	
}value;

typedef struct _kvpair	{
	key* k;
	value* v;
	struct _kvpair* next;
}kvpair;

typedef struct _bucket	{
	kvpair* head;
	int size;
}bucket;

typedef struct _ht	{
	bucket* buckets;
	int size;
}ht;

typedef struct _htta	{
	ht** hlist;
	int sizeeach;
	int n;
}htta;

typedef struct _htinfo	{
	int usedbucket;
	int nkeys;
	double avgk_valid;
	double avgk_all;
}htinfo;

//--------key
key* key_copy(key* k);
int key_cmp(key* v1, key* v2);
key* key_random(int len);


//--------value
value* value_copy(value* v);
int value_cmp(value* v1, value* v2);

//--------kvpair
kvpair* kvp_new(key* k, value* v);



//--------ht
ht* ht_new(int size);
void ht_put(ht* h, key* k, value* v);
value* ht_get(ht* h, key* k);
void ht_remove(ht* h, key* k);


void ht_getinfo(ht* h, htinfo* hi);


//--------htta
htta* htta_new(int sizeeach, int n);
void htta_put(htta* h, key* k, value* v);
value* htta_get(htta* h, key* k);
//void htta_remove(htta* h, key* k);



#endif

