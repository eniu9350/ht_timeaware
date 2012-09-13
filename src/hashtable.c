#include "hashtable.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

//--------key

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

key* key_copy(key* k)
{
	int len = strlen(*k);
	key kcopy = MALLOC((len+1), char);
	strcpy(kcopy, *k);
	key* p_kcopy = MALLOC(1, key);
	*p_kcopy = kcopy;
	return p_kcopy;
}

int key_cmp(key* v1, key* v2)
{
	return strcmp(*v1, *v2);
}

static char genRandom()
{
    return alphanum[rand() % stringLength];
}

key* key_random(int len)
{
	key k = MALLOC((len+1), char);
	int i;
	for(i=0;i<len;i++)	{
		k[i] = genRandom();
	}
	key* pk =  MALLOC(1, key);
	*pk = k;
	return pk;
}

//--------value
value* value_copy(value* v)
{
	return v;
}

int value_cmp(value* v1, value* v2)
{
	return 0;
}

//--------kvpair
kvpair* kvp_new(key* k, value* v)
{
	kvpair* kvp = MALLOC(1, kvpair);
	value* vcopy = value_copy(v);
	key* kcopy = key_copy(k);
	kvp->k = kcopy;
	kvp->v = vcopy;
	kvp->next = NULL;
	return kvp;
}


//--------ht

ht* ht_new(int size)
{
	int i;
	ht* h;
	h = MALLOC(1, ht);
	h->buckets = MALLOC(size,bucket);
	h->size = size;
	for(i=0;i<size;i++)	{
		h->buckets[i].head = NULL;
	}
	return h;
}

static void ht_bucket_add(ht* h, int id, key* k, value* v)
{
	bucket* b = h->buckets+id;
	kvpair* head;
	kvpair* tail;	//last before tail
	if(id>=h->size)	{
		perror("ht_bucket_add id>=h->size!");
		return;
	}

	kvpair* kvp = kvp_new(k, v);
	
	head = b->head;
	if(head==NULL)	{
		b->head = kvp;
	}else	{
		tail = head;
		while(tail->next!=NULL)	{
			tail = tail->next;
		}
		tail->next = kvp;
	}

	b->size ++;
	
}

static value* ht_bucket_get(ht* h, int id, key* k)
{
	bucket* b = h->buckets+id;
		kvpair* head;
		kvpair* tail; //tail
		if(id>=h->size)	{
		perror("ht_bucket_get id>=h->size!");
		return NULL;
	}

	head = b->head;
	
	tail = head;
	while(tail!=NULL)	{
		if(key_cmp(tail->k, k)==0)	{
			return tail->v;
		}else	{
			tail = tail->next;
		}
	}
	
	return NULL;
}

static void ht_bucket_remove(ht* h, int id, key* k)
{
	bucket* b = h->buckets+id;
		kvpair* head;
		kvpair* p; //tail
		kvpair* prev;
		if(id>=h->size)	{
		perror("ht_bucket_remove id>=h->size!");
		return NULL;
	}

	head = b->head;
	
	prev = p = head;
	while(p!=NULL)	{
		if(key_cmp(p->k, k)==0)	{
			//found, remove
			if(p==head)	{	//set head pointer
				b->head = p->next;
			}else	{
				prev->next = p->next;
			}

			b->size --;
			return;
		}else	{
			prev = p;
			p = p->next;
		}
	}
	
	printf("ht_bucket_remove error: not found! k=%s\n", *k);
}


void ht_put(ht* h, key* k, value* v)
{
	int hashid = (int)((hash(*k))%(h->size));
	//printf("hash *k = %ld, h->size=%d\n", hash(*k), h->size);
	ht_bucket_add(h, hashid, k, v);
}

value* ht_get(ht* h, key* k)
{
	int hashid = (int)((hash(*k))%(h->size));
	//printf("hash *k = %ld, h->size=%d\n", hash(*k), h->size);
	return ht_bucket_get(h, hashid, k);
}

void ht_remove(ht* h, key* k)
{
	int hashid = (int)((hash(*k))%(h->size));
	//printf("ht_remove: hash *k = %ld, h->size=%d\n", hash(*k), h->size);
	return ht_bucket_remove(h, hashid, k);
}

void ht_getinfo(ht* h, htinfo* hi)
{

	int i;
	bucket* b;

	hi->nkeys = 0;
	hi->usedbucket = 0;
	for(i=0;i<h->size;i++)	{
		b = h->buckets+i;

		if(b->size!=0)	{
				hi->nkeys += b->size;
				hi->usedbucket ++;
		}
	}

	hi->avgk_valid = hi->nkeys*1.0/hi->usedbucket;
	hi->avgk_all = hi->nkeys*1.0/h->size;
}

//--------htta
htta* htta_new(int sizeeach, int n)
{
	int i;
	htta* h = MALLOC(1, htta);
	ht* hitem;
	h->hlist = MALLOC(n, ht*);
	for(i=0;i<n;i++)	{
		h->hlist[i] = ht_new(sizeeach);
	}
	h->n = n;
	h->sizeeach = sizeeach;
	return h;
}

void htta_put(htta* h, key* k, value* v)
{
	int hashid = (int)((hash(*k))%(h->sizeeach));
	//printf("hash *k = %ld, h->size=%d\n", hash(*k), h->size);
	ht* hitem;
	//mmm:
	hitem = h->hlist[0];
	ht_bucket_add(hitem, hashid, k, v);
}

value* htta_get(htta* h, key* k)
{
	int hashid = (int)((hash(*k))%(h->sizeeach));
	//printf("hash *k = %ld, h->size=%d\n", hash(*k), h->size);
	//mmm: some heuristics
	ht* hitem;
	hitem = h->hlist[0];
	return ht_bucket_get(hitem, hashid, k);
}

void htta_remove(htta* h, key* k)
{
	int hashid = (int)((hash(*k))%(h->sizeeach));
	value* v;
	//printf("ht_remove: hash *k = %ld, h->size=%d\n", hash(*k), h->size);
	//mmm: some heuristics
	ht* hitem;
	hitem = h->hlist[0];
	//check if exist in it
	//mmm: try remove directly
	v = ht_bucket_get(hitem, hashid, k);
	if(v==NULL)	{
		//mmm: check others
	}
	return ht_bucket_remove(h, hashid, k);
}


void htta_getinfo(htta* h, htinfo* hi)
{

	int i,j;
	bucket* b;
	ht* hitem;

	hi->nkeys = 0;
	hi->usedbucket = 0;

	for(i=0;i<h->n;i++)	{
		hitem = h->hlist[i];
		for(j=0;i<hitem->size;j++)	{
			b = h->buckets+j;

			if(b->size!=0)	{
					hi->nkeys += b->size;
					hi->usedbucket ++;
			}
		}
		}

	hi->avgk_valid = hi->nkeys*1.0/hi->usedbucket;
	hi->avgk_all = hi->nkeys*1.0/((h->n)*(h->sizeeach));
	
}

