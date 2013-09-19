/* ===============================================================================================*/
/**                                                                               
   \file pitch_yin.c
   \brief Observation operations
*/    

/* =================================================================================================


/* This algorithm was developped by A. de Cheveigne and H. Kawahara and
 * published in:
 * 
 * de Cheveigne, A., Kawahara, H. (2002) "YIN, a fundamental frequency
 * estimator for speech and music", J. Acoust. Soc. Am. 111, 1917-1930.  
 *
 * see http://recherche.ircam.fr/equipes/pcm/pub/people/cheveign.html
 */



/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

#include "pitch_yin.h"

#include <stdlib.h>

/*#ifdef VECLIB
##include <veclib/cblas.h>
##include <veclib/clapack.h>
##else
##include <cblas.h>
#include <clapack.h>
#endif
*/
#include <atlas/clapack.h>
#include <atlas/cblas.h>

/*==================================================================================================
                                     LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/



/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                                      LOCAL VARIABLES
==================================================================================================*/
                                                            
            

/*==================================================================================================
                                     GLOBAL VARIABLES
==================================================================================================*/



/*==================================================================================================
                                 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
                                                             


/*==================================================================================================
                                     LOCAL FUNCTIONS
==================================================================================================*/



/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/


/** outputs the difference function 
 * \param *input        (i) [0..N-1] input buffer
 * \param N             
 * \param *diff         (i) [0..lag-1] difference function
 * \param lag
 * \return Nothing
 */
void pitch_yin_diff(float * input, int N, float * diff, int lag){
  int j,tau;
  float tmp;
  
  for (tau=0; tau<lag; tau++){
    diff[tau] = 0.;
  }
  for (tau=1; tau<lag; tau++){
    for (j=0;j<N-lag;j++){
      tmp = input[j] - input[j + tau];
      diff[tau] += tmp * tmp;
    }
  }
}

/** cumulative mean normalized difference function 
 * \param *diff 
 * \param N
 * \return Nothing
 */
void pitch_yin_getcum(float * diff, int N) {
  int tau;
  float tmp;
  tmp = 0.;
  diff[0] = 1.;
  for (tau=1; tau<N; tau++){
    tmp += diff[tau];
    diff[tau] *= tau/tmp;
  }
}


/** pitch estimator
 * \param *cumdiff
 * \param N
 * \return Pitch lag for voiced frame, PITCH_YIN_UNVOICED for unvoiced frame
 */
int pitch_yin_getpitch(float * cumdiff, int N) {
  int tau=1;
  do {
    if(cumdiff[tau] < PITCH_YIN_THR) { 
      while (cumdiff[tau+1] < cumdiff[tau] && tau+1 < N) {
	tau++;
      }
      return tau;
    }
    tau++;
  } while (tau < N);
  return PITCH_YIN_UNVOICED; /* Unvoiced */
}

/** Pitch estimator with quadratic interpolation
 * \param pitch Value of  the pitch found by getpitch
 * \param *diff Difference buffer computed with pitch_yin_diff
 * \param N size of the diff buffer
 *
 * \return the pitch in sub sample precision
 */
float pitch_yin_getfpitch(int pitch, float *diff, int N){
  float varout = 0.0;
  int i;
  
  /* For LAPACK */
  long int order = 3;
  long int numvec = 1;
  long int linfo = 0;

  long int *ipiv;                                /* For storing pivot indices */
  float *X;
  float *Y;
  float *A;

  if(pitch == PITCH_YIN_UNVOICED){
    return varout;
  }

  if(N - pitch <= PITCH_YIN_QUADEST){
    return varout;
  } 

  if(pitch - PITCH_YIN_QUADEST < 0){
    return varout;
  }


  X = malloc(sizeof(float) * 3 * (2*PITCH_YIN_QUADEST+1));
  if(X == NULL){
    return varout;
  }

  A = malloc(sizeof(float) * 3 * 3);
  if(A == NULL){
    free(X);
    return varout;
  }
  ipiv = malloc(sizeof(int) * 3);
  if(ipiv == NULL){
    free(A);
    free(X);
    return varout;
  }
  Y = malloc(sizeof(float) * 3);
  if(Y == NULL){
    free(A);
    free(ipiv);
    free(X);
    return varout;
  }


  for( i = 0 ; i < ( 2 * PITCH_YIN_QUADEST + 1 ) ; i++ ){
    X[i] = 1.0;
    X[i+(2*PITCH_YIN_QUADEST+1)] = - PITCH_YIN_QUADEST + i;
    X[i+2*(2*PITCH_YIN_QUADEST+1)] = (- PITCH_YIN_QUADEST + i)*(- PITCH_YIN_QUADEST + i);
  }

  cblas_sgemv(CblasColMajor, CblasTrans, 
	      2*PITCH_YIN_QUADEST+1, 3, 
	      1.0, X, 2*PITCH_YIN_QUADEST + 1, 
	      diff + pitch - PITCH_YIN_QUADEST, 1, 
	      0.0, Y, 1);
  cblas_sgemm(CblasColMajor, CblasTrans, CblasNoTrans, 
	      3, 3, 2*PITCH_YIN_QUADEST + 1, 
	      1.0, X,  2*PITCH_YIN_QUADEST + 1, 
	      X,  2*PITCH_YIN_QUADEST + 1, 
	      0.0, A, 3);

#ifdef USE_ATLAS
  clapack_sgesv(CblasColMajor, order, numvec, A, order, ipiv, Y, order);
#else
  /* Standard fortran interface */
  sgesv_(&order, &numvec, A, &order, ipiv, Y, &order, &linfo);
#endif

   varout = - Y[1] / ( 2 * Y[2] );

  free(X);
  free(A);
  free(ipiv);
  free(Y);
  
  return varout;
}
