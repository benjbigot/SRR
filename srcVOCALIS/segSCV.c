/*============================================================================
					--Vocalic Segmentation--
Based on the Segmenation Code of F.Pellegrino and R. André-Obrecht
Name        : segSCV.c
Author      : Ben Bigot
Version     : 1.1
Copyright   : benbigot
Description : vocalic segmentation in C
============================================================================*/
/*
 version 1.1 : add list of segments management

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>
#include <getopt.h>
#include "segments.h"
#include "systeme.h"
#include <math.h>
#include "pitch_yin.h"
//#include <stdlib.h>

#include <atlas/cblas.h>
#include <atlas/clapack.h>
//#ifdef VECLIB
//#include <veclib/cblas.h>
//#include <veclib/clapack.h>
//#else
//#include <cblas.h>
//#include <clapack.h>
//#endif

/*====================== variables globales =======================*/
float F_HZ , F_KHZ;
int verbose;
int vPitch;
int OutPrint;
/*=================================================================*/
float *getAudioContent(char* filePath){
	/* description  : load audio file to a double 
	 then returns pointer to array					*/
	
	if (verbose){
		puts("==========================================================");
		puts("----------------- Loading data ---------------------------");	
		puts("==========================================================");
	}
	
	SNDFILE		*inFile; 	/* pt on the inputfile 		*/
	SF_INFO     sfInfo ; 	/* header struct		*/
	sf_count_t 	readCount ;	/* sample			*/
	sf_count_t	status;		/* to check audio loading 	*/
	float		*audio;		/* audio data container		*/

	if (! (inFile =  sf_open( filePath,  SFM_READ , &sfInfo))){
        	printf ("Not able to open input file %s.\n", filePath) ;
	        sf_error(inFile) ;
	}
	else{
		if (verbose){ printf("loading audio file : %s\n", filePath);}
		if (sfInfo.channels != 1){
			printf("the file contains %i channels... ciao\n", sfInfo.channels);
		}
		else{
			/*printf("the file contains %i samples\n", (int) sfInfo.frames);*/
			if ((audio = malloc((sfInfo.frames + 1)  * sizeof(float))) == NULL){
				puts("memory allocation problem");
			}
			else{
				readCount = (sf_count_t) sfInfo.frames;
				/* sfInfo.frame == sfInfo.section if sfInfo.channel == 1  */
				status = sf_readf_float(inFile, audio, readCount) ;
				if (status != readCount){
					puts("problem while reading audio data...");
				}
				else{
					if(verbose){ printf("%i audio frames loaded...\n", (int) readCount);}
				}
			}
		}
	}
	/* bidouille inhérente au code de la segmentation FB
	 decalage d'une case pour coller aux contrainte */
	float * audioStaggered = malloc(( sfInfo.frames + 10 ) * sizeof(float));
	int kk ;
	for (kk = 0; kk <= readCount ; kk ++){
		audioStaggered[kk+1] = audio[kk];
	}

	free(audio);


	return audioStaggered;
}
/*=================================================================================*/
struct SF_INFO getAudioInfo(char* filePath){
	/*	description : returns audio file informations	*/
	SNDFILE		*inFile; 	/* pt on the inputfile 		*/
	SF_INFO     sfInfo ; 	/* header struct			*/

	if (! (inFile =  sf_open( filePath,  SFM_READ , &sfInfo))){
		printf ("Not able to open input file %s.\n", filePath) ;
		sf_error(inFile) ;
	}
	else{
		if (verbose){ printf("audio file %s info loaded...\n", filePath);}
	}
	
	// bb integration du test sur le nombre de channel
	if (sfInfo.channels != 1){
		puts("---> can only process single channel audio, bye...");
		exit(1);
	}

	F_HZ		=	(float) sfInfo.samplerate ;
	F_KHZ		= 	F_HZ / 1000.0 ;	
	
	return sfInfo;
}
/*====================================================================================*/
float *loadFilter(char* filterPath){
	/* description : chargement des coefficients du filtre 
	 avec test de la parité et récupération du nb de coefficients 
	 l'ordre du filtre pourrait être augmenté						*/
	if (verbose)
	{
		puts("==========================================================");
		puts("--------- loading low-band filter coefficients -----------");
		puts("==========================================================");
	}
	
	float	*filter = NULL;	
	FILE	*f_filter = NULL;
	int 	nCoeffFilter;
	int		filterLength;
	int		symetricParity;
	float	symetricNegWeigth = -1.0; // default value for even parity of coefficients
	int i;	

	if ((f_filter = fopen(filterPath, "r"))==NULL){
			printf("unable to open filter file \"%s\" \n", filterPath);
			exit(1);
	}
	else{
		fscanf(f_filter, " %d %d\n", &nCoeffFilter, &symetricParity); // first line informations
		filterLength = 2 * nCoeffFilter ;
		filter = malloc(sizeof(float) * (filterLength+1));

		if (symetricParity == 0) symetricNegWeigth = 1.0 ; /*odd impulse response filter*/

		for ( i = 1; i <= nCoeffFilter; i++ ){
			fscanf(f_filter, "%e", &filter[i]);
			filter[filterLength - i + 1] = symetricNegWeigth * filter[i];
		}
		fclose(f_filter);
				
		// printing output informations 
		if (verbose){ 
			puts("filter coefficients loaded...");
			printf("filter order : %i\n", filterLength);
		}
		if (symetricParity == 1){
			if (verbose){ puts("transient response : even symmetry");}
		}
		else{
			if (verbose){ puts("transient response : odd symmetry");}
		}
	}
	return  filter;
}
/*======================================================================*/
float* normalizeCenter(float *audioContent, int lengthBuff ){	
	/* description : centering and normalizing the whole audio file */	
	if (verbose){
		puts("==========================================================");
		puts("---------------- Normalizing signal ----------------------");	
		puts("==========================================================");	
	}
	
	float max = 0 ;
	int i ;
	float *audioNormalized ;
	audioNormalized = malloc(sizeof(float) * (lengthBuff + 1));
	float cumulate = 0;
	
	/* getting mean value */
	for (i = 1 ; i <= lengthBuff ; i++){
		cumulate += audioContent[i];
	}
	cumulate /= lengthBuff;
	
	/* centering */
	for (i = 1 ; i <= lengthBuff ; i++){
		audioNormalized[i] = audioContent[i] - cumulate ;
	}	

	/* getting max of centered signal */
	for (i = 1 ; i <= lengthBuff ; i++){
		if (fabs(audioNormalized[i]) > max ){
			max = fabs(audioNormalized[i]);
		}
	}
	
	/* normalizing */
	for (i = 1 ; i <= lengthBuff ; i++){
		audioNormalized[i] /= max;
	}
	free(audioContent);
	if (verbose){
		puts("Normalizing Process achieved"); 
	}
	
	return audioNormalized;
}
/*==============================================*/
void printAudioInfo( char * filePath , char * filterPath, char * outDir, float startTime, float duration, SF_INFO audioInfo){
	if (verbose){
		puts("=========================================================");
		puts("----------------- segmentation SCV -----------------------"); 
		puts("==========================================================");
		printf("audio file path 	: \"%s\"\n", filePath);
		printf("filter file 		: \"%s\"\n", filterPath);
		printf("output directory	: \"%s\"\n", outDir);
		//printf("start time		: %f second(s)\n", startTime);
		//printf("segment length		: %f second(s)\n", duration );
		puts("---------------- Audio Info ------------------------------");
		printf("the file contains 	: %i channel(s)\n", audioInfo.channels);
		printf("file samplerate 	: %i Hertz\n", audioInfo.samplerate);
		printf("the file is seekable 	: %i\n", audioInfo.seekable);
		printf("the file contains 	: %i section(s)\n", audioInfo.sections);
		printf("the file contains 	: %i frame(s)\n", (int)audioInfo.frames);
										   
	}
}								   
/*================================================================*/
void usage(){
	puts("./segSCV < -i wavfile> \n\
	< -l list of segment to process [nbSegment \n  start duration]> \n\
	< -s debut>\n\
	< -d duration>\n\
	< -v verbose mode> \n\
	< -p print pitch only >\n\
	< -w wavesurfer format> else <outfileBasename STDIN>");
}
/*================================================================*/
int checkSegment(float startSample, float durationSample, SF_INFO audioInfo){
	if (startSample>= 0 && ( startSample  + durationSample - 1) <= (audioInfo.frames-1)){
		if (verbose){ 
			//printf("start sample     :  %i\n", startSample ); 
			return 1 ;
		}
		//				if (durationSample == 0){
		//					durationSample = audioInfo.frames; // for whole signal processing
		//				}
		//				if (verbose){ printf("duration samples :  %i\n", durationSample);}
	}
	else{
		puts("please check start and duration parameters");
		exit(1);
	}
	
}
/*================================================================*/
int getNbLine(char * listePath){
	int nbLine;
	if ( listePath != NULL){	
		FILE * fList = NULL ;		
		int j=0;
		fList = fopen(listePath , "r");
		fscanf(fList, "%i" , &nbLine);
		fclose(fList);

	}
	else{
		nbLine = 1 ;
	}
	return nbLine;
}
/*=================================================================*/
float * getDebut(char * listePath, float startTime, SF_INFO audioInfo, int nbLine){
	if ( listePath != NULL){	
		FILE * fList = NULL ;		
		int j=0;
		float temp = 0;
		float * debut = NULL ;
		
		fList = fopen(listePath , "r");
		fscanf(fList, "%f" , &temp);
		
		
		debut = malloc(sizeof(float) * nbLine);
		for (j = 0; j< nbLine ; j++)	{
			fscanf(fList, "%f %f\n", &debut[j], &temp);
			debut[j] = debut[j] *   audioInfo.samplerate ;
			//printf("%f %f\n", debut[j], temp);
		}
		
		fclose(fList);		
		return debut; 
	}
	else{
		int j = 0;
		float *debut = NULL;
		debut = malloc(sizeof(float));
		for (j = 0; j< nbLine ; j++)	{
			debut[j] = startTime * audioInfo.samplerate; 
		}
		return debut;
	}
	
	return 0 ;
}
/*===================================================================*/
float * getDuree( char * listePath, float duration , SF_INFO audioInfo, int nbLine){
	if ( listePath != NULL){	
		FILE * fList = NULL ;		
		int j=0;
		float temp = 0;
		float * duree = NULL ;
		fList = fopen(listePath , "r");
		fscanf(fList, "%i" , &nbLine);
		
		duree = malloc(sizeof(float) * nbLine);
		for (j = 0; j< nbLine ; j++)	{
			fscanf(fList, "%f %f\n",  &temp, &duree[j]);
			duree[j] = duree[j] *  audioInfo.samplerate ;
			//printf("%f %f\n", temp, duree[j]);
		}
		fclose(fList);
		return duree ; 
	}
	else{
		int j = 0;
		float *duree = NULL;
		duree = malloc(sizeof(float));
		for (j = 0; j< nbLine ; j++)	{
			duree[j] = duration * audioInfo.samplerate; 
		}
		return duree;
	}
	
	return 0 ;
}
/*================================================================*/
/*
int checkSegmentList(char * listePath, float ** debut , float ** duree  , float startTime, float duration, SF_INFO audioInfo){
	int nbLine;	
	if (verbose){
		puts("==========================================================");
		puts("--------- checking start and duration parameters ---------");
		puts("==========================================================");		
	}
	
	if ( listePath != NULL){	
		FILE * fList = NULL ;		
		int j=0;
		fList = fopen(listePath , "r");
		fscanf(fList, "%i" , &nbLine);
		
		debut = malloc(sizeof(float*) * nbLine);
		duree = malloc(sizeof(float*) * nbLine);
			
		for (j = 0; j< nbLine ; j++)	{
			fscanf(fList, "%f %f\n", &debut[j], &duree[j]);
			printf("%f %f\n", debut[j], duree[j]);
		}
		
		fclose(fList);
		
		printf("%i\n", &debut[0]);
		puts("ca marche");
				
		
		int i = 0, startSample, durationSample;
		for (i = 0 ; i <= nbLine ; i++){
			
			startSample 	= (int) &debut[i] * audioInfo.samplerate ; // startingTime -> startingSample
			puts("---------->ca marche<------------");
			
			durationSample 	= (int) &duree[i] * audioInfo.samplerate; // duration -> sample slice			
			if (checkSegment(startSample, durationSample, audioInfo)) ;			
		}	
		
	}
	
	else{
		debut = (float **) malloc(sizeof(float*)); 
		duree = (float **) malloc(sizeof(float*));
		nbLine = 1 ;
		
		debut[0] 	= (int) startTime * audioInfo.samplerate ; //  startingTime -> startingSample
		duree[0] 	= (int) duration * audioInfo.samplerate; // duration -> sample slice
		checkSegment( (int) &debut[0], (int) &duree[0], audioInfo);
		// ili faudrait presque initialiser une liste de 1 valeur pour simplifier le process
	}
	puts("------------------>testing");
	puts("segments seem ok");
	return nbLine ;
}
*/
/*================================================================*/
float * produceBuffer(float * audioContent, float startSample, float durationSample, SF_INFO audioInfo){
	if (verbose){
		puts("==========================================================");
		puts("----------------- creating input buffer ------------------");
		puts("==========================================================");		
	}	
	
	float * processingAudio = malloc(sizeof(float) * (durationSample+1)); 
	memcpy(processingAudio, audioContent, sizeof(float)*(durationSample+1));
	durationSample = (int) audioInfo.frames;
	return(processingAudio);
}
/* ------------------------- synopis -------------------------- */
/* charger les débuts et fins de segments dans une liste a partir d'un fichier texte avec entete */
/* vérifier la validité des segments */
/* charger le fichier wav */
/* procéder à l'extraction pour chacun des segments */
/* modification importante concernant la gestion de la liste de segment (utilisation de fscanf() */
/* mise en forme des sorties pitch et segSCV */

/*=====================__main__===================================*/
int main(int argc, char **argv) {
	char 	*filePath 			= NULL;
	char	*listePath			= NULL;
	char 	*outDir				= "./results/" ;
	char 	*filterPath 		= "./filt4.dat";

	/*filter frequencies overs 4kHz for a sampling frequency = 12.8kHz, order 128 */
	float	startTime 			= 0.0 ;
	float	duration			= 0.0 ;
	int 	startSample			= 0 ;
	int 	durationSample		= 0 ;
	
	// table de float chargée à partir du fichier listeSegmentToProcess
	float * debut = NULL ;
	float * duree = NULL ;
	int nbLine = 0 ;

	// audioBuffer
	float 	*audioContent 		= NULL;
	float   *audioContentNormalized = NULL ;
	float 	*filterContent		= NULL;
	struct  SF_INFO				audioInfo;		
			verbose				= 0; //variable globale
	int		wavesurfer			= 0;
	float * processingAudio	= NULL;
	
	//  Divergence buffers
	int		*outDivSegmentation	=	NULL; // Forward-Backward output
 	int 	nbSegmentDiv			=	0;

	// SAD buffer
	double		facteur		= 4.0 ; // pour l'algo adaptatif ?
	int			pausemin	= 50  ; // pour la durée mini d'une pause ?
	double		intervalle	= 1.0 ; // paur l'intervalle mini considéré ?
	int			*segSAD		= NULL; 

	// Pitch extraction
	float		*bufferPitch = NULL ;
	
	// segSCV
	int		*sortieSegSCV	= NULL ;	
	int		freq_rate		= 0 ; 	//rapport de fréquences par rapport à 8kHz	
	int		facteurFreq		= 1.0 / 0.1 ;			// O.1 est VOW_Threshold intialement.				
	int 	idSeg;

	
	//--------- input parameters ---------
	int 	option_index 			= 0 ;
	int 	c ;	
	static struct option long_option[] =
		{
			{"audio"		, required_argument	, NULL	, 'i' },
			{"start"		, required_argument	, NULL	, 's' },
			{"duration"		, required_argument	, NULL	, 'd' },
			{"liste"		, required_argument	, NULL	, 'l' },
			{"verbose"		, no_argument		, NULL	, 'v' },
			{"wavesurfer"	, no_argument		, NULL	, 'w' },
			{"pitch"		, no_argument		, NULL	, 'p' },
			{"out"			, no_argument		, NULL	, 'o' }
		};
		
	// args parsing
	while ((c = getopt_long(argc,argv, "i:s:d:l:vwpo", long_option, &option_index))!=-1){
		switch(c){
			case 'i':
				filePath = malloc(sizeof(char)*strlen(optarg));
				sprintf(filePath,"%s",optarg);
				break;
			case 'l':
			    listePath = malloc(sizeof(char)*strlen(optarg));
				sprintf(listePath, "%s", optarg);				
			case 's':
				startTime = atof(optarg);
				break;
			case 'd':
				duration = atof(optarg);
				break;
			case 'v':
				verbose = 1 ;
				break;
			case 'w':
				wavesurfer = 1;
				break;
			case 'p' :
				vPitch = 1 ;
				break ;
			case 'o' :
				OutPrint = 1 ; 
				break;
			case '?' :
				usage();
				exit(1);
			default :
				usage();
				exit(1);
		}		
	}

	//==============================================================//	
	//_________________________PROCESSING___________________________//
	//==============================================================//
	
	/*---- extract file info and check number of channel -----*/
	audioInfo = getAudioInfo(filePath);
	
	/*------------ Print audio Info -------------------------------*/
	printAudioInfo(filePath, filterPath, outDir, startTime, duration, audioInfo);			   				
	
	/*_____________ Load and check segment list _________________*/
	nbLine = getNbLine(listePath);
	debut  = getDebut(listePath, startTime, audioInfo, nbLine );
	duree  = getDuree(listePath, duration, audioInfo, nbLine ); 
	
	
	//nbLine = checkSegmentList(listePath, debut, duree,  startTime, duration, audioInfo);
	//printf("%i\n", nbLine);
	//printf("%f %f\n", debut[0], duree[0] );
	// retourne une liste de segment triée dans debut, duree et nbLine
	
	/*============ load audio signal ==============================*/
	audioContent  = getAudioContent(filePath);		
	
	/*=============== normalize === ===============================*/
	audioContentNormalized = normalizeCenter(audioContent,(int)audioInfo.frames);	

	/*====================== chargement du filtre ================================*/
	filterContent = loadFilter(filterPath);
	//free(audioContent);
	
	/*-----------------------------------------------------------------------------*/
	/*========================== process each segment =============================*/
	for ( idSeg = 0 ; idSeg < nbLine; idSeg++){
		//printf("processing segment %i :%f %f \n", idSeg, debut[idSeg], duree[idSeg]/16000);

		/*======================== Producing sub-buffer ===========================*/
		processingAudio = produceBuffer(audioContentNormalized, debut[idSeg], duree[idSeg], audioInfo);

		/*======================= divergence Forward Backward =====================*/				
		outDivSegmentation = divergFB(processingAudio, duree[idSeg], filterContent, audioInfo.samplerate);
		
		
		//int yy =0 ;
		//for ( yy = 0 ; yy <= 787 ; yy++){
		//	printf("%d\n"  , outDivSegmentation[yy]);
		//}
		//printf("%f\n", duree[idSeg]);
		while (outDivSegmentation[nbSegmentDiv] < (int)duree[idSeg]){ 
			//printf("%i %i  %f \n",nbSegmentDiv , outDivSegmentation[nbSegmentDiv],  duree[idSeg] );    
			nbSegmentDiv++;	
		}			
		//puts("test");
		
		if (verbose){ printf("nb of segments found : %i\n", nbSegmentDiv+1);} 	// nb de segment (nbSegmentDiv+1) for number 
		//exit(1);
		
		/*====================== Voice Activity detection =========================*/
		// (nbSegmentDiv+1) = nombre de segment divFB
		//printf("%f\n", duree[idSeg]);
		segSAD = detecter_silences(processingAudio, duree[idSeg],  outDivSegmentation, (nbSegmentDiv+1),  facteur, pausemin, intervalle,  audioInfo.samplerate );			
		
		/*===================== Pitch extraction ==================================*/
		bufferPitch =  extractPitch( processingAudio, duree[idSeg], vPitch, audioInfo );

		/*===================== SCV segmentation ==================================*/
		freq_rate = audioInfo.samplerate / 8000 ;
		sortieSegSCV = detection(processingAudio,  audioInfo.samplerate,  (int) duree[idSeg], freq_rate, facteurFreq, outDivSegmentation, segSAD, bufferPitch,  (nbSegmentDiv+1));		

		/*===================== Post processing ===================================*/
		postProcessing(sortieSegSCV, outDivSegmentation, segSAD,  nbSegmentDiv, wavesurfer);
		
		
	}
	
	//free(processingAudio);
	//free(outDivSegmentation);
	//free(bufferPitch);
	//free(sortieSegSCV);
	//free(audioContentNormalized);


	
	return EXIT_SUCCESS;
}
