/*
 *  postProcessing.c
 *  
 *
 *  Created by Benjamin Bigot on 04/09/12.
 *  Copyright 2012 Université Paul Sabatier. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>

extern int verbose;
extern float F_HZ, F_KHZ;
extern int OutPrint;
	
//------------------------------------------------------	
int postProcessing(int * sortieSegSCV, int * outDivSegmentation, int * segSAD, int nbSegmentDiv, int wavesurfer){	
	if (verbose){
		puts("Vocalic Segmentation achieved");
		puts("==========================================================");		
		puts("------------------- Post Processing ----------------------");
		puts("==========================================================");
	}
	
	
	int nbVoyelle = sortieSegSCV[0];
	if (verbose){
		printf("nb voyelle final : %i\n", nbVoyelle);
	}
	int uuu;
	char label;
	
	
	if (wavesurfer || OutPrint){
		
		if (outDivSegmentation[0] > 0){
			if (wavesurfer){
				printf("0 %f #\n",(float)outDivSegmentation[0]/F_HZ);
			}
			else{
				printf("0 #\n");
			}
		}
		
		for ( uuu = 0; uuu < ( nbSegmentDiv  ) ; uuu++ ){
			
			if (segSAD[uuu] == -5){
				label = 'V';
			}
			else if (segSAD[uuu] == -1 || segSAD[uuu] == 1 ){
				label = 'C';
			}
			else{
				label = '#';
			}		
			
			// la ligne suivante permet de générer une version adaptée à WaveSurfer
			if (wavesurfer){
				printf("%f %f %c\n",  (float)outDivSegmentation[uuu]/ F_HZ,(float)outDivSegmentation[uuu+1]/ F_HZ ,label);
			}
			else{
				printf("%f %c\n",  (float)outDivSegmentation[uuu]/ F_HZ ,label);
			}
			
		}
	}
}
