#ifndef _PITCH_YIN_H_
#define _PITCH_YIN_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ===============================================================================================*/
/**                                                                            
  \file pitch_yin.h
  \brief YIN pitch estimator
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


/*==================================================================================================
                                         CONSTANTS
==================================================================================================*/


/*==================================================================================================
                             TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                          MACROS
==================================================================================================*/


/*==================================================================================================
                                     GLOBAL VARIABLES
==================================================================================================*/
#define PITCH_YIN_THR 0.3              /* Voiced/Unvoiced thresold for cmnd */

//#define PITCH_YIN_UNVOICED -1          /* Unvoiced pitch */
#define PITCH_YIN_UNVOICED 0          /* Unvoiced pitch */
	
#define PITCH_YIN_QUADEST 3            /* *2+1 = amount of points used for quadratic estimation */


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
void pitch_yin_diff(float * input, int N, float * diff, int lag);

/** cumulative mean normalized difference function 
 * \param *diff 
 * \param N
 * \return Nothing
 */
void pitch_yin_getcum(float * diff, int N);

/** pitch estimator
 * \param *cumdiff
 * \param N
 * \return Pitch lag for voiced frame, PITCH_YIN_UNVOICED for unvoiced frame
 */
int pitch_yin_getpitch(float * cumdiff, int N);

/** Pitch estimator with quadratic interpolation
 * \param pitch Value of  the pitch found by getpitch
 * \param *diff Difference buffer computed with pitch_yin_diff
 * \param N size of the diff buffer
 *
 * \return teh pitch in sub sample precision
 */
float pitch_yin_getfpitch(int pitch, float *diff, int N); 

/*================================================================================================*/
#ifdef __cplusplus
}
#endif
#endif /* _PITCH_YIN_H_ */
