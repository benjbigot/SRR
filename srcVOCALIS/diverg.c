#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "segments.h"
#include <math.h>

/*-------------------------------------------------------------------------*/
float 	COR[22]	; 
float 	VARF[22];
float 	VARB[22];
float 	RESF[22];
float 	RESB[22];
float 	RC[22]	;
float 	R[22]	;
float 	XA[1024];

extern int verbose;

/*--------------------------------------------------------------------*/
void AUTOV(int N,int IAU,int M,float *ALPH, float *XAU){
	// calcul de l'autocorrélation
	
	float AU[22],RCA[22],VALPH,VXAU;
	int MQ,K,NK,NP,I;
	int MINC,IB,IP,MH;
	float AT,S;

	VALPH	= 0.;
	VXAU 	= *XAU;
	MQ	= M+1;

	if(IAU == 0){
		for ( K=2; K <= MQ; K=K+1){
			R[K] = R[K] - VXAU * XA[K-1] + XA[N-K+1] * XA[N];
		}
		R[1] = R[1] - VXAU * VXAU + XA[N] * XA[N];
	}
	else{
		for ( K = 1; K <= MQ ; K=K+1 ){
			R[K]=0.;
			NK=N-K+1;
			for ( NP =1; NP <= NK; NP=NP+1){
				R[K] = R[K] + XA[NP] * XA[NP+K-1] ;
			}
		}
	}

	
	
	if (R[1] <= 0.){
		R[1]=1.;
	}

	RCA[1]	= -R[2]/R[1];
	AU[1]	= 1.;
	AU[2]	= RCA[1];
	VALPH	= R[1]+R[2]*RCA[1];

	if (M > 1){
		for ( MINC = 2; MINC <= M && VALPH > 0 ; MINC=MINC+1){
			S=0.;
			for ( IP=1; IP <= MINC; IP= IP+1){
				S=S+R[MINC-IP+2]*AU[IP];
			}
			RCA[MINC]= -S/VALPH;
			MH=MINC/2+1;

			for ( IP=2; IP <= MH; IP = IP+1){
				IB=MINC-IP+2;
				AT=AU[IP]+RCA[MINC]*AU[IB];
				AU[IB]=AU[IB]+RCA[MINC]*AU[IP];
				AU[IP]=AT;
			}

			AU[MINC+1]=RCA[MINC];
			VALPH=VALPH+RCA[MINC]*S;
		}
	}

	if (VALPH > 0.){
		VALPH=VALPH/ (float)(N-1);
		*ALPH=VALPH;
		VXAU=0.;

		for ( I=1; I<= MQ; I=I+1){
			VXAU = VXAU + AU[I] * XA[N-I+1] ;
		}
		*XAU= VXAU;
	}
}

/*-----------------------------------------------*/
void INDIR1 (int inf,int sup,float* sig_float){
	/* INVERSION DU SIGNAL ENTRE DEUX 
	 * ECHANTILLONSinf ET sup EUX MEMES INCLUS */
	int i, j, k;
	float tmp;
	j = (int) ( (sup - inf + 1) / 2 ) + inf - 1;

	for ( i = inf, k = 0; i <= j; i++, k++){
		tmp = sig_float[i];
		sig_float[i] = sig_float[sup-k];
		sig_float[sup-k] = tmp;
	}
}

/*--------------------------------------------------------------*/
void TREILV (int K, float Y,int M,float *VARA, float *RES){
	/* DETERMINATION DU MODELE LONG TERME
	 PROGRAMME D'IDENTIFICATION DU MODELE AR PAR LA METHODE DE BURG */

	float	RESBP[22];
	float	GAINV;
	float	GAINC;

	int 	IK;
	int		I;
	int		KK;

	if (Y == RESF[1]){
		Y=Y+1.;
	}

	GAINV		= 1./(float)(K);
	VARF[1]		= VARF[1]+GAINV*(Y*Y-VARF[1]);
	VARB[1]		= VARF[1];
	RESF[1]		= Y; 
	IK			= M;
	

	if ( (K-1) < M ){ //M = ordre, utilisé pour le gain
		IK	= K-1;
	}

	if(K != 1){
		for ( I=1; I <= IK; I=I+1){
			RESBP[I]	= RESB[I];
		}
		for ( I=1; I <= IK; I=I+1){
			KK			= K-I;
			GAINC		= 1./(float)(KK);
			COR[I]		= COR[I] + GAINC * (RESBP[I] * RESF[I] - COR[I]);
			RC[I]		= 2. * COR[I] / (VARF[I] + VARB[I] );
			RESF[I+1]	= RESF[I]-RC[I]*RESBP[I];
			RESB[I+1]	= RESBP[I]-RC[I]*RESF[I];
			VARF[I+1]	= VARF[I+1]+GAINC*(RESF[I+1]*RESF[I+1]-VARF[I+1]);
			VARB[I+1]	= VARB[I+1]+GAINC*(RESB[I+1]*RESB[I+1]-VARB[I+1]);
		}
	}

	RESB[1]	= Y;
	*VARA	= VARF[IK+1];
	*RES	= RESF[IK+1];
}

/*----------------------------------------------------------------------------*/

int DIVH1V(int NINI, int NFIN, float B,float LAM,int *NRUPT0,int NLIM,float NFECH,int M, float* SIGNAL){

	/* PROGRAMME DE SEGMENTATION POUR LE SIGNAL INVERSE */
	//  LAM passé en paramètre jamain utilisée
	
	float MAXI,Z1,ZAU,X2,VARA,RES,ALPH,XAU;
	int N1,N0,KM1,I,NRUPT,IAU,NFFRUP;
	int KMIN,KPL,MQ;
	float LAMV,BV,U,QV;
	int fini, flag31;

	KMIN=(int) (NFECH*20.);	// fenetre de 20 ms 
	NFFRUP = NFIN-10;
	BV= -.2;
	LAMV= 40.;
	NRUPT= NINI;
	*NRUPT0= NINI;
  
	/* CREATION DES DONNEES ET INITIALISATION DU TRACE */
	MQ=M+1;
	KM1=KMIN-1;
	ALPH=0.;

	INDIR1(NINI,NFIN,SIGNAL);
	N1=NINI-1;
	N0=N1;

	/*INITIALISATIONS SUR UNE PLAGE */
	fini = 0;
	do{
		for ( I=1; I <= 21; I=I+1){
			VARF[I]=1.;
			VARB[I]=1.;
			RESF[I]=0.;
			RESB[I]=0.;
			COR[I]=0.;
		}
		KPL=0;
		ZAU=0.;
		Z1=0.;
		VARF[MQ]=1.;
		VARB[MQ]=1.;
		MAXI=0.;
		N1=N0;
		N0=N0+KMIN;

		/*   TRAIT. D'UNE PLAGE    */
		flag31 = 0;
		while ( !flag31 ){
			N1=N1+1;
			if(N1 > NFFRUP){
				fini = 1;
				break;
			}
			KPL=KPL+1;
			Z1=0.;
			X2=SIGNAL[N1];
			TREILV(KPL,X2,M,&VARA,&RES);
			if(KPL <= 0){
				ZAU=Z1;
				continue;
			}
			
			if(KPL > KMIN){
				IAU=0;
				XAU=XA[1];
				for ( I=1; I <= KM1; I=I+1){
					XA[I]=XA[I+1];
				}
				XA[KMIN]=SIGNAL[N1];
			}
			else{
				XA[KPL]=SIGNAL[N1];
				if(KPL < KMIN){
					ZAU=Z1;
					continue;
				}
				IAU=1;
				XAU=0.;
			}
			AUTOV(KMIN,IAU,M,&ALPH,&XAU);

			/* CALCUL DU TEST -DIVERGENCE-HINKLEY */
			QV=ALPH/VARA; 
			U=(2.*XAU*RES/VARA-(1.+QV)*RES*RES/VARA+QV-1.)/(2.*QV);
			Z1=ZAU+U-BV;

			/* DERNIER POINT OU LE MAX A ETE ATTEINT */

			if (MAXI <= Z1){
				MAXI=Z1;
				N0=N1;
			}
			/* SEUIL DE RUPTURE */

			if((MAXI-Z1) < LAMV){
				ZAU=Z1;
				continue;
			}

			if ((NFIN-N0) > NLIM){
				*NRUPT0=NRUPT;
				NRUPT=N0;
			}
			flag31 = 1;

		}
	} while ( ! fini );
    	
	INDIR1(NINI,NFIN,SIGNAL);
	NRUPT= NINI - NRUPT + NFIN;
	*NRUPT0= NINI - *NRUPT0 + NFIN;
	return ( NRUPT);
}

/*--------------------------------------------------------**/
int  DIVB(float *sig_filt, int N2,int NMAX,float NFECH){
		
	// dans le méthode de la divergence une rupture est recherchée entre des modèles 
	// estimés à court et à long terme
	// le long terme est estimé dans une fenêtre de taille croissante (tous les points 
	// depuis le début du signal ou depuis la dernière rupture)
	// le modele a court terme est estimé dans une fenetre modbile de taille fixe
	// le modele a long terme est calculé en utilisant l'algorithme de burg (treilv())
	// le modele a court terme est calculé par le méthode dite d'autocorrelation (autov())
	// a chaque instant les deux AR sont estimés et on calcule la distance par le test de kinley.
		
	// le detecteur exploite les inovations des deux filtres (residus ici ??) ainsi que les 
	// variances (varf et varb ??)
	
	// il faut distinguer le kmin utiliser pour parcourir l'ensemble du signal 
	// et celui qui sert à calculer les divergences à partir d'une rupture.
	// dans le calcul de divergence d'est 20ms ou 1/8 de 20ms soit 2,5 ms
	

	
	int		KMIN	= (int) ( NFECH * 20.0 ) / 8 ;  
	/* ici vaut 1/8eme du nb de point par fenetre d'analyse avec une fenetre de 20 ms*/
	
	/*-- initialisation sur une plage --*/
	float 	ZAU	= 0.0 ;
  	float 	MAXI	= 0.0 ;
	float	Z1;
	float	X2;
	float	VARA;
	float	QV;
	float	ALPH;
	float	LAM	= 80.0 ;
	float	B	= -0.8 ;
	float	XAU	;
	float	RES ;
	float	U ;

	int 	KPL	= 0 ;
	int	M	= 4 ;
	int	IAU;
	int 	MQ	= M + 1 ;
	int 	KM1	= KMIN - 1 ; 
	int	N1	= N2 - 1 ;
	int 	N0 	= N1 ;
		N0	= N2 + KM1 ;
	int 	i;
	int 	I;


	for ( i =1; i <=21; i=i+1){
		VARF[i]		= 1.0 ; 
		VARB[i]		= 1.0 ;
		RESF[i]		= 0.0 ;
		RESB[i]		= 0.0 ;
		COR[i]		= 0.0 ;
    	}

	VARF[MQ]	= 1.0 ;
	VARB[MQ]	= 1.0 ;

	/*---- traitement d'une plage ----*/
	do{
		N1=N1+1;
		if (N1 > NMAX){
			N0=N1-1;
			return ( N0 ); 
		}
		KPL=KPL+1;
		Z1=0.;
		X2=sig_filt[N1]; 
		
		TREILV(KPL,X2,M,&VARA,&RES);

		if (KPL > KMIN){ 
			// on décale les données vers la gauche de manière à parcourir le signal.
			IAU=0;
			XAU=XA[1];
			for ( I=1 ; I <= KM1; I=I+1){
				XA[I]=XA[I+1];
			}
			XA[KMIN]=sig_filt[N1];
			
			// mise à jour du vecteur XA utilisé pour la méthode d'autovariance
			// XA est une table[1024] non initialisée
		}
		else{ // sinon on initialise et concatène les valeurs dans XA (révèle un transitoire)
			// puisque KPL est initialisé à 0.
			XA[KPL]=sig_filt[N1];
			if (KPL < KMIN){ 
				ZAU=Z1; 
				continue;
			}
			IAU=1;
			XAU=0.;
		}  
		
		AUTOV(KMIN,IAU,M,&ALPH,&XAU);
		QV=ALPH/VARA;
		
		/* TEST -DIVERGENCE-HINKLEY**/
		if (QV <= 0.){
			U = 0.;
		}
		else{
			U=(2.*XAU*RES/VARA-(1.+QV)*RES*RES/VARA+QV-1.)/(2.*QV); 
		}
		Z1=ZAU+U-B; 
	
		/* DERNIER POINT OU LE MAX A ETE ATTEINT */
		if (MAXI <= Z1){ 
			MAXI=Z1;
			N0=N1;
		}		
		if ( (MAXI-Z1) < LAM ) { 
			ZAU=Z1;
		}
	} while ( (MAXI-Z1) < LAM ); 
	// si seuil dépassé -> fin de boucle, return position

	return ( N0 ); 
}
/*-------------------------------------------------------------------*/
/*Filtrage du signal dans domaine temporel par produit de convolution*/
float* FILTRA(float *sig_float, int NMAX, float *H){
	
	float *sig_filt = malloc(sizeof(float) * (NMAX+1));
	int i,j;
	int NFILT;
	int NN ;
	int M;
	float YA;
	NFILT=128;
	NN=NFILT;

/*
	for ( i=1; i < NFILT; i++ ){
		YA=0.;
		for ( j = 1; j < i; j++ ){
			YA = YA + ( H[j] * sig_float[i-j] );
		}
		sig_filt[i]=YA;
	}
	for ( NN = (NFILT+1) ; NN <= NMAX; NN = NN+1 ){
		YA = 0.;
		for ( M = 1; M <= NFILT; M++){
			YA = YA + (H[M] * sig_float[NN-M] );
		}
		sig_filt[NN] = YA;
	}
*/

/*------ autre proposition vérifiée par appli numérique */
 
	for ( i=1; i <= NMAX; i++ ){
		YA=0.0;
		for ( j = 1; j <= NFILT; j++ ){
			if ((i-j)>=0){
				YA = YA + ( H[j] * sig_float[i-j+1] );
			}
		}
		sig_filt[i]=YA;
	}

	return sig_filt;
}



/*=====================================================================================*/
/*	Fonction principale de Divergence Forward-Backward			*/
int *divergFB(float* audio,int durationSample,float* filterCoeff,int samplerate){

	if (verbose){
		puts("==========================================================");
		puts("----------- Segmentation Forward Backward ----------------");
		puts("==========================================================");
	}

	float	*SIGNAL			= audio	;					/* tranche de signal considéré */
	float	analysisW		= 0.01 ;					/* analysis windows in second*/
	int	KMIN				= analysisW * samplerate; 	/* fenetre analyse 160 samples (10ms)*/
	int	NBSEGMAX			= durationSample / KMIN; 	/* utilisé pour initialiserles tables */
	int 	NMAX			= durationSample ;			/* nécessaire? */
	float*	filteredSignal	= malloc(sizeof(float) * (NMAX+1));
	int *outSegmentation = NULL; // function's output
	
	int	model_order 	= 2 ; /* indiqué "en dur" dans cette version*/

	int	*NUM		= malloc(sizeof(int) * (NBSEGMAX+1)) ;
	int	*NUMF		= malloc(sizeof(int) * (NBSEGMAX+1)) ;

	
	
	char 	*TEMP		= malloc(sizeof(char) * (NBSEGMAX+1));
	float	NFECH		= 	samplerate / 1000.0 ; /* en kHz  pas joli*/

	float 	LAM 		= 40.0;	
	float 	B			= -0.2;
	float 	ALPH		=  0.0;

	int 	NLIM		= 1.5 * KMIN ;
	int		BMAX		= (2 * KMIN) +1 ;
	int 	NBMAX		= 5 * KMIN ;
	int		MQ		= model_order + 1;
	int 	KM1		= KMIN - 1;

	/*-------- filtrage du signal-----------*/
	filteredSignal	= FILTRA(audio, durationSample, filterCoeff);
	
	/*--------- divergence ----------------*/
	int 	I;
	int 	rupture_directe;
	int		fini;
	int 	KPL;
	int 	N1	= 0 ;
	int		N0	= N1 ;
	int		N2	= 1 + N1 ;
	int		NRUPT	= N1 ;
	int		NRUPT0	= N1 ;
	int 	IAU;
	int 	FN2	= 1 ;		
	int		FNN	= 0 ;		
	int 	NALF	= 1 ;
	int 	NAL	= 0 ; 
	int		J;


	float	ZAU;
	float 	MAXI;
	float 	VARA, RES, XAU;
	float 	U, QV, Z1, XX2;
	
	FNN = DIVB ( filteredSignal , FN2, NMAX, NFECH);

	NUMF[1] = FNN;
	fini = 0;
		
	while (! fini ){
		KPL	= 0;
		ZAU	= 0.;
		MAXI	= 0.;
		
		for ( I=1; I <= 21; I++){
			VARF[I]	= 1.;
			VARB[I]	= 1.;
			RESF[I]	= 0.;
			RESB[I]	= 0.;
			COR[I]	= 0.;
		}
		VARF[MQ]	= 1.;
		VARB[MQ]	= 1.;
		N1		= N0;
		N2		= N0+1;
 		N0		= N2 + KMIN - 1;
		
		/*traitement du segment courant*/ 
		rupture_directe = 0 ;
		while ( ! rupture_directe ){
			N1++;
			if(N1 > NMAX){
				break;
			}

			KPL++;
			Z1	= 0.;

			XX2	= SIGNAL[N1];			
			TREILV(KPL,XX2, model_order,&VARA,&RES); 

			if(KPL < 0){
				ZAU	= Z1;
				continue;
			}
			
			if(KPL > KMIN){
				IAU	= 0;
				XAU	= XA[1];
				I	= 0;
				for ( I=1;  I <= KM1; I++) {
					XA[I]	= XA[I+1];
				}
				XA[KMIN]	= SIGNAL[N1];
			}
			else{
				XA[KPL]		=SIGNAL[N1]; 
				if(KPL < KMIN ){
					ZAU	= Z1;
					continue;
				}
				IAU	= 1;
				XAU = 0.;
			}
			AUTOV(KMIN,IAU,model_order,&ALPH,&XAU);
			
			
			/***********************************
			 CALCUL DU TEST -DIVERGENCE-HINKLEY
			***********************************/
			
			QV	= ALPH/VARA;
			U	= (2. * XAU * RES / VARA - (1. + QV) * RES * RES / VARA + QV - 1.) / (2. * QV);
			Z1	= ZAU + U - B;

			if (MAXI <= Z1){
				MAXI	= Z1;
				N0		= N1;
			}
			
			if((MAXI-Z1) < LAM){ 
				ZAU	= Z1;
			}
			else{
				rupture_directe = 1;
			}

			
		} // fi (rupture_directe)
	
		/*RUPTURE DE TEST*/
		
		if ( ( (FN2-N2) >= BMAX ) && ( FN2 <= N0 ) ) {
			N0	= FN2;
		}
		
		if( ( FNN <= N0 ) && ( (FNN-N2) >= BMAX ) ) {
			N0	= FNN;
		}
		
		NRUPT	= N0;
		KPL		= N0-N2+1;
				
		if(KPL > NBMAX){ 
			
			NRUPT=DIVH1V(N2,N0,B,LAM,&NRUPT0,NLIM,NFECH,model_order, SIGNAL); 
		}

				
		N0		= NRUPT;
		NAL		= NAL+1;
		NUM[NAL]= N0; 
		if(N0 == FNN){
			TEMP[NAL]= 'b'; 
		}
		

		if(FN2 <= N0){
			FN2			= FNN+1;			
			FNN 		= DIVB(filteredSignal, FN2, NMAX, NFECH);
			NALF		= NALF+1;
			NUMF[NALF]	= FNN; 			
		}
		
		if ((N0+KMIN) > NMAX){ 
			fini = 1;
		}
	} // fi (fini)
	
	/*ENREGISTREMENT DES FRONTIERES NUM CONTIENT LES FRONTIERES VALIDEES */  
	NAL=NAL+1;
	NUM[NAL]= NMAX;
	if (verbose){
		printf("Forwards Backwards Segmentation done ! %i segments........\n", NAL - 1);
	}
	
  /********************************************/
 /*		Fin de traitement du fichier		*/
/*******************************************/
	outSegmentation = malloc(( NAL + 1) * sizeof(int));
	//puts("1 Forwards Backwards Segmentation done........");
	//printf("%i\n", NAL);
	//puts("2 Forwards Backwards Segmentation done........");

	for (J=1; J<= NAL; J++){
		outSegmentation[J-1] = NUM[J];
	}
	//puts("3 Forwards Backwards Segmentation done........");

	return outSegmentation;
}
