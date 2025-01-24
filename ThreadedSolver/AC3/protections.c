#include "AC3.h"

pthread_mutex_t qtex;
pthread_cond_t rcond, vcond;
sem_t revising, verifying;

struct revisor rtab[NUM_VALUES];
struct verifier vtab[NUM_SLOTS];

uint8_t finished = FALSE;
uint8_t rflag = FALSE;

status ac3init()
{
    status retval = SUCCESS;
    retval |= pthread_mutex_init(&qtex, NULL);

    retval |= pthread_cond_init(&rcond, NULL);
    retval |= pthread_cond_init(&vcond, NULL);

    retval |= sem_init(&revising, 0, NUM_VALUES);
    retval |= sem_init(&verifying, 0, NUM_NEIGHBORS);

    retval |= rinit();
    retval |= vinit();

    return retval;
}

status ac3free()
{
    status retval = SUCCESS;
    retval |= pthread_mutex_destroy(&qtex);

    retval |= pthread_cond_destroy(&rcond);
    retval |= pthread_cond_destroy(&vcond);

    retval |= sem_destroy(&revising);
    retval |= sem_destroy(&verifying);

    retval |= rfree();
    retval |= vfree();

    return retval;
}

status rinit()
{
    status retval = SUCCESS;
    for (tid8 r_idx = 0; r_idx < NUM_VALUES; r_idx++) {
        rtab[r_idx].self = r_idx + 1;
        retval |= pthread_mutex_init(&rtab[r_idx].rtex, NULL);
    }
    return retval;
}

status rfree()
{
    status retval = SUCCESS;
    for (tid8 r_idx = 0; r_idx < NUM_VALUES; r_idx++)
        retval |= pthread_mutex_destroy(&rtab[r_idx].rtex);
    return retval;
}

status vinit()
{
    status retval = SUCCESS;
    for (tid8 v_idx = 0; v_idx < NUM_SLOTS; v_idx++) {
        vtab[v_idx].self = v_idx;
        retval |= pthread_mutex_init(&vtab[v_idx].vtex, NULL);
    }
    return retval;
}

status vfree()
{
    status retval = SUCCESS;
    for (tid8 v_idx = 0; v_idx < NUM_SLOTS; v_idx++)
        retval |= pthread_mutex_destroy(&vtab[v_idx].vtex);
    return retval;
}