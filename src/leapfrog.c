#ifdef __cplusplus
extern "C" {
#endif
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "misc.h"
#include "leapfrog.h"
#include "matvec.h"

#define MAX_N 1000

static int N;
static double T, DT, *R, *V, internal_r[MAX_N], internal_v[MAX_N];
static FF_EVAL F;

/*
** n is the number of dimensions.  r and v are each n-dimensional.
*/
void init_leapfrog(int n, double t0, double dt, double *r, double *v, FF_EVAL f)
{
    double A[n];
    int i;
    N = n;
    T = t0;
    DT = dt;
    R = r;
    V = v;
    assert(0 < n && n <= MAX_N);
    VecCopy(n, internal_r, R);
    VecCopy(n, internal_v, V);
    F = f;

    /*
    ** advance velocities by 1/2 step
    */
    F(N, T, internal_r, A);
    for(i=0; i < N; i++)
	internal_v[i] += DT/2 * A[i];
}


/*
** return actual tout.  Note: T and tout refer to the time of the positions;
** the velocities are ahead by half a step.
*/
double integrate_leapfrog(double tout)
{
    double A[N];
    int i;

    if(tout == T)
	return T;

    assert(tout >= T);

    /* Algorithm: as long as the positions are at a time
     * less than tout, keep going.  Since T represents the time of the positions,
     * and the velocities are half a step ahead after each loop iteration, it's
     * possible the velocities could be *past* tout when we're done.  That's OK.
     */
    while(T + DT <= tout)
    {
	for(i=0; i<N; i++)
	    internal_r[i] += DT * internal_v[i];
	T += DT;
	F(N, T, internal_r, A);
	for(i=0; i < N; i++)
	    internal_v[i] += DT * A[i];
    }

    /* Now we're close.  Take a weird mutant baby Euler step to tout.
     * The velocities are half a timestep ahead of the positions, so
     * their mutant timestep is shorter by DT/2.
     */
    for(i=0; i<N; i++)
	R[i] = internal_r[i] + (tout - T) * internal_v[i];
    F(N, tout, R, A);
    for(i=0; i < N; i++)
	V[i] = internal_v[i] + (tout - (T+DT/2)) * A[i];

    return tout;
}
#ifdef __cplusplus
} // end extern "C"
#endif
