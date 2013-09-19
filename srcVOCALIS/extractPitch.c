/*
 *  extractPitch.c
 *  extractPitch function
 *
 *  Created by Benjamin Bigot on 04/09/12.
 *
 */


#include "pitch_yin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>


//#ifdef VECLIB
//#include <veclib/cblas.h>
//#include <cblas.h>
//#include <lapack.h>
//#include <veclib/lapack.h>
//#else
//#include <cblas.h>
#include <atlas/cblas.h>
//#include <clapack.h>
#include <atlas/clapack.h>
//#endif

extern int verbose;
//-------------------------------------------------//

float * extractPitch(  float *  processingAudio, float durationSample, int vPitch , SF_INFO audioInfo ){
	
	
	
	
	
	
	/* Configuration options */
	int framesize = 160; // 10ms pour f_eq = 16000Hz
	int buffersize = 400 ;//  160 par défaut;
	int plag = 360 ; // PMIN = 100  par défaut; /* pitchlag */		
	int i, j;
	
	if (verbose){
		puts("==========================================================");
		puts("----------------- Pitch Extraction -----------------------");
		puts("==========================================================");		
		printf("Pitch YIN configuration :\n");
		printf("Signal buffer		: %d\n", buffersize);
		printf("Frame size		: %d\n", framesize);
		printf("Pitch lag		: %d\n", plag);	
	}
	
	
	/* Audio buffer */
	float *buffer; 
	int frame_count;	
	
	/* Parameter output */
	float *diff;                     /* Difference function */
	float *cdiff;                    /* For the cumulativ difference function */
	int pitch;
	float fpitch;
	float * bufferPitch = NULL;  // pitch container
	

	frame_count = durationSample/framesize; // pour le sous signal
	bufferPitch = malloc(sizeof(float) * frame_count); 
	
	buffer = malloc(sizeof(float) * buffersize);
	if(buffer == NULL){
		puts("error while initializing buffer");
		exit(1);
	}		
	memset(buffer, 0, sizeof(float) * framesize); 
	
	diff = (float *) malloc(sizeof(float) * plag);
	if(diff == NULL){
		free(buffer);
		puts("error while initializing diff");
		exit(1);
	}
	memset(diff, 0, sizeof(float) * plag);
	
	cdiff = (float *) malloc(sizeof(float) * plag);
	if(cdiff == NULL){
		free(diff);
		free(buffer);
		puts("error while intiializing cdiff");
		exit(1);
	}
	memset(cdiff, 0, sizeof(float) * plag);
	
	/* compute pitch */
	for(i=0; i<frame_count; i++){
		for(j=0; j<buffersize-framesize; j++){
			buffer[j] = buffer[j+framesize];
		}
		memcpy( &buffer[buffersize-framesize], &processingAudio[(i*framesize)+1], sizeof(float)*framesize);
		pitch_yin_diff(buffer, buffersize, diff, plag);
		memcpy(cdiff, diff, sizeof(float)*plag);
		pitch_yin_getcum(cdiff, plag);
		pitch = pitch_yin_getpitch(cdiff, plag);
		
		fpitch = (float) pitch + pitch_yin_getfpitch(pitch, diff, plag);
		
		if (fpitch <= 0.0){
			fpitch =  0.0;
		}
		else{
			fpitch = 1.0 /fpitch * audioInfo.samplerate; 
		}
		
		bufferPitch[i] = fpitch ;
		
		if (vPitch){
			printf("%f\n", bufferPitch[i]);
		}
		// le pitch présente des sauts de valeurs importants:
		// proposer un lissage des donneés
	}
	
	free(buffer);
	free(diff);
	free(cdiff);
	
	
	
	return bufferPitch ;
	
	
	
	
	
	
	
	
	
}
