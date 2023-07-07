#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <omp.h>

#define MAX_POINTS 2000


typedef struct vec2_s {
    double x, y;
} vec2_type;

double vec2_dist_sq (vec2_type u, vec2_type v) {
    double diff_x = u.x - v.x;
    double diff_y = u.y - v.y;
    return diff_x*diff_x + diff_y*diff_y;
}

typedef struct vec2set_s {
    vec2_type v[MAX_POINTS];
    int size;
} vec2set_type;

void vec2set_read (vec2set_type* set) {
    vec2_type u;
    set->size = 0;
    while (scanf ("%lf %lf",&(u.x),&(u.y)) == 2) {
        if (set->size < MAX_POINTS) {
            set->v[set->size] = u;
            set->size += 1;
        } else {
            printf ("Too many points in stdin\n");
            exit(1);
        }
    }
}

double center_cost (vec2set_type* set, int c1, int c2, int c3) {
    double cost = 0;
    for (int i=0;i<set->size;i++) {
	double dist_sq_1 = vec2_dist_sq (set->v[i],set->v[c1]);
	double dist_sq_2 = vec2_dist_sq (set->v[i],set->v[c2]);
	double dist_sq_3 = vec2_dist_sq (set->v[i],set->v[c3]);
	double min_dist_sq = dist_sq_1;
	if (dist_sq_2 < min_dist_sq) {
	    min_dist_sq = dist_sq_2;
	}
	if (dist_sq_3 < min_dist_sq) {
	    min_dist_sq = dist_sq_3;
	}
	if (min_dist_sq > cost) {
	    cost = min_dist_sq;
	}
    }
    return cost;
}

int main (int argc, char** argv) {

    if(argc != 2) { 
        printf("Command usage: %s %s \n",argv[0],"num_threads");
        return 1;
    } 

        /* get number of threads from command line (optional argument) */
    int num_threads = atoi(argv[1]); 
    omp_set_num_threads(num_threads);

    /* start the timer */
    double start_time, end_time;
    start_time = omp_get_wtime();

       vec2set_type set;
    vec2set_read (&set);
    double optimal_cost = DBL_MAX;
    int optimal_c[3];

    int tuples_checked = 0; 

    

#pragma omp parallel shared (optimal_cost, tuples_checked, optimal_c, set) 
 {

int tuples_checked_thread = 0;  
int thread_num = omp_get_thread_num(); 
int thread_c[3]; 
double optimal_cost_thread = DBL_MAX; 

#ifdef DYNAMIC 
#pragma omp for schedule(dynamic)
#else
#pragma omp for schedule(static)
#endif 

    /* solve the 3-center problem exactly */
    for (int i=0;i<set.size-2;i++) {
	for (int j=i+1;j<set.size-1;j++) {
	    for (int k=j+1;k<set.size;k++) {
		tuples_checked_thread += 1;
		double cost_thread = center_cost (&set, i, j, k);
		if (cost_thread < optimal_cost_thread) {
		    optimal_cost_thread = cost_thread;
		    thread_c[0] = i;
		    thread_c[1] = j;
		    thread_c[2] = k;
		}
	    }
	}
    }
#pragma omp critical
    {
        tuples_checked += tuples_checked_thread;
        if(optimal_cost_thread < optimal_cost){
            optimal_cost = optimal_cost_thread;
            optimal_c[0] = thread_c[0];
            optimal_c[1] = thread_c[1];
            optimal_c[2] = thread_c[2]; 
        } 
    } 
#ifdef DEBUG 
    printf("Thread %d checked %d tuples\n", thread_num, tuples_checked_thread); 
#endif 
 } 
 end_time = omp_get_wtime(); 

#ifdef DEBUG 

    
    /* print out the number of 3-tuples checked */
    printf ("total number of 3-tuples checked = %d\n",tuples_checked);

    /* print the minimal cost for the 3-center problem */
    printf ("minimal cost = %g\n",optimal_cost);
  
    /* print an optimal solution to the 3 center problem */
    printf ("optimal centers : %d %d %d\n",optimal_c[0],optimal_c[1],optimal_c[2]);

    printf ("wall time used = %.4f sec\n",(end_time-start_time));
 #else
    printf ("(%d,%.4f),",num_threads,(end_time-start_time)); 

#endif

   return 0;
} 
