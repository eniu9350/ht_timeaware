#include <stdio.h>
#include <stdlib.h>
#include <gsl_rng.h>
#include <gsl_randist.h>
#include <time.h>

#include "common.h"

/*
http://xifage.com/generate-random-numbers-using-gsl-1/
*/


#include "hashtable.h"



typedef struct _session	{
	key* id;
	double start;
	double end;
	value* v;
	unsigned int flag;
}session;





int main(int argn, char* argv[])
{
	int i,j,k,id;



	//loop params
	double now;
	double end;
	double step;
	
	int nuser = 200000;
	double mean_ia = 0.01;
	double mean_srv = 30; //mean service time

	//-----ht
	int ht_bucksize = 1000; //each
	int ht_nway = 3;
	int ht_buckperiod = 10; //each(realtion with u?)
	ht* h;

	//----ht init
	h = ht_new(ht_bucksize*ht_nway);

	gsl_rng *r;
	const gsl_rng_type *T;
	int n=5;
	double u;
		
	T=gsl_rng_ranlxs0;		//设随机数生成器类型是 ranlxs0
	
	
	//gen arrival
	gsl_rng_default_seed = ((unsigned long)(time(NULL))); 	 //设seed值为当前时间
	r=gsl_rng_alloc(T); 	 //生成实例
	
	double* exp_sample_ir = MALLOC(nuser, double);
	for(i=0;i<nuser;i++)	{
			exp_sample_ir[i] = gsl_ran_exponential(r, mean_ia);
			//printf("exp: %f\n", exp_sample_ir[i]);
	}
	
	
	double* arrival_real  = MALLOC(nuser, double);
	arrival_real[0] = 0.0;
	for(i=1;i<nuser;i++)	{
		arrival_real[i] = arrival_real[i-1]+exp_sample_ir[i-1];
		//printf("arrival_real: %f\n", arrival_real[i]);
	}


	printf("\n\n");

	//gen service time	//mmm: alloc right???
	gsl_rng_default_seed = ((unsigned long)(time(NULL)+200)); 	 //设seed值为当前时间
	r=gsl_rng_alloc(T); 	 //生成实例

	
	double* exp_sample_srv = MALLOC(nuser, double);
	for(i=0;i<nuser;i++)	{
		exp_sample_srv[i] = gsl_ran_exponential(r, mean_srv);
		//printf("srv: %f\n", exp_sample_srv[i]);
	}


	//gen session
	session* sessions = MALLOC(nuser, session);
	end = 0;
	for(i=0;i<nuser;i++)	{
		id = i;
		sessions[id].id = key_random(10);
		sessions[id].start = arrival_real[i];
		sessions[id].end = sessions[id].start + exp_sample_srv[i];
		if(sessions[id].end>end)	{
			end = sessions[id].end;
		}
		sessions[id].flag = SESSION_FLAG_INIT;
		sessions[id].v = MALLOC(1, value);
		//printf("session %d(%f~%f)\n", id, sessions[id].start, sessions[id].end);
	}


	//start event loop
	now = 0;
	//end = 10000;
	step = 1.0;
	//mmm: init session and its value
	session* s;
	htinfo* hi = MALLOC(1, htinfo);
	printf("before loop, end=%f\n", end);
	while(now<end)	{
		j=0;
		k=0;
		for(i=0;i<nuser;i++)	{
			s = sessions+i;
			if(now>=s->start && s->flag==SESSION_FLAG_INIT)		{
					//printf("before put, id=%s\n", *(s->id));
					ht_put(h, s->id, s->v);
					s->flag |= SESSION_FLAG_ADD;
					j++;
			}
			if(now>=s->end && s->flag==(SESSION_FLAG_INIT|SESSION_FLAG_ADD))		{
				ht_remove(h,s->id);
				s->flag |= SESSION_FLAG_DELETE;
				k++;
			}
		}
		
		now+=step;

		//---print info
		if(((int)now)%20!=0)	{
			continue;
		}
		ht_getinfo(h, hi);
		printf("now=%f", now);
		if(j!=0)	{
			printf(", added %d", j);
		}
		if(k!=0)	{
			printf(", deleted %d", k);
		}
		printf(", htinfo--- keys=%ld, used=%d/%d, avgkeys(valid buck)=%f, lf=%f", hi->nkeys, hi->usedbucket, h->size, hi->avgk_valid, hi->avgk_all);
		printf("\n");
	}
	
	
/*
  int i,n;
  double gauss,gamma;  

  n=atoi(argv[1]);
  for (i=0;i<n;i++)
    {
      gauss=gsl_ran_gaussian(r,2.0);
      gamma=gsl_ran_gamma(r,2.0,3.0);
      printf("%2.4f %2.4f\n", gauss,gamma);
    }
*/
	
	printf("aaa\n");
	

}
