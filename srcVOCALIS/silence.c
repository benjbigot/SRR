/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/** Date de Creation :  31/05/00                                        **/
/** Date de derniere modif :  12/06/2012								**/
/** Systeme : unix														**/
/** Langage : C                                                         **/
/** Auteur : FP                                                         **/
/**---------------------------------------------------------------------**/
/** Description :Procedures de detections des pauses                    **/
/**  inspirees de la version matlab                                     **/
/**---------------------------------------------------------------------**/
/** Journal des versions :                                              **/
/**                                                                     **/
/**     Date         |      Remarques                                   **/
/** -----------------|--------------------------------------------------**/
/** 31/05/00         |       v0                                         **/
/** 18/07/03         |       Debuggage {-1 0} => {-1 -1}                **/
/** 18/07/03         |       Debuggage {-1 1 -1} => {-1 -1 -1}          **/
/** 18/07/03         |       par ajout d'une deuxième passe             **/
/** 12/06/2012       |       BB adaptation ˆ segSCV                     **/
/**                  |                                                  **/
/** -----------------|--------------------------------------------------**/
/*************************************************************************/
/*************************************************************************/

#include "systeme.h"

/*************************************************************************/
/***		DECLARATION DES VARIABLES EXTERNES	                       ***/
/*************************************************************************/

// global variables to be added
//extern int F_HZ; 
//extern int F_KHZ;
extern int verbose;

/*************************************************************************/
/***		PROCEDURES                                                 ***/
/*************************************************************************/

// Procedure calcul_std
float calcul_std(float * Sig, int Deb, int Fin){
	double sumx, sumx2, valeur, mini, maxi;
	int i,n;

	sumx = 0;
	sumx2 = 0;
	n = Fin-Deb + 1;
	mini = 1e15;
	maxi = -1e15;

	for (i=Deb;i<=Fin;i++){
		sumx += Sig[i];
		sumx2 += Sig[i]*Sig[i];
		if (Sig[i]<mini){
			mini = Sig[i];
		}
		if (Sig[i]>maxi){
			maxi = Sig[i];
		}
	}	
	valeur = sqrt(((n*sumx2-sumx*sumx)/n)/(n-1));
	return(valeur);	
}
	
/*******************************************************/
/*		PROCEDURE detecter_silences ADAPTATIF 		*/
/*******************************************************/
// Segments samples from 0 to nbseg
int* detecter_silences(float *tableau, int NbSig, int *segments, int nbseg, double Facteur, int PauseMin, double Interval_duration, int F_HZ){
	if (verbose){
		puts("==========================================================");
		puts("--------------- Speech Activity Detection ----------------");
		puts("==========================================================");
	}
	int		i,j;
	int		* etiquettes;
	int		fenetre_courante, trame_max;
	float	ecart_min;
	int		debut, fin, new_deb, new_fin;
	double	* ecarts;
	double	* min_intervalle;
	int		* longueurs;
	float	seuil;
	int		longueur_cumulee;
	int		ten_prct;
	int		intervalle;
	float	F_KHZ		= 	(float) F_HZ / 1000.0 ;
	
	
	
	// VALEUR DE L'INTERVALLE ADAPTATIF : demi seconde
	intervalle = (int) floor (Interval_duration * 1000 * F_KHZ / 2.0);
	if (verbose){
		printf("interval duration is %.2f \n", Interval_duration);
	}
	
	etiquettes = (int *) malloc( (nbseg+1) * sizeof(int));	
	ecarts			= (double *) malloc(nbseg * sizeof(double));
	min_intervalle	= (double *) malloc( (nbseg) * sizeof(double));
	longueurs		= (int *) malloc( (nbseg) * sizeof(int));

	if ((ecarts  == NULL) || (min_intervalle == NULL)|| (longueurs == NULL)){
		printf("Erreur : DŽpassement mŽmoire en Allocation des tableaux ecarts et/ou longueurs\n");
		exit(1);
	}

		
	
	ecart_min = 1e6;
	for (i=0;i<nbseg;i++){
		min_intervalle[i] = ecart_min;
	}
	/**********************************************************/		
	// PHASE 1 : CALCUL DES ECARTS POUR CHAQUE SEGMENT
			
	ecarts[0]		= calcul_std(tableau, 1, segments[0]-1); // pour le premier segment
	longueurs[0]	= segments[0]-1;
	
	// pour les segments suivant ; strictement infŽrieur ˆ
	for ( i = 1 ; i < nbseg ; i++ ){
		debut	= segments[i-1]		;
		fin		= segments[i] - 1	;
		longueurs[i] = fin - debut  ;
		// seuil sur les longueurs en echantilons dŽpendant de la frŽquence d'Žchantillonage.
		if (longueurs[i] > 20){
			ten_prct	= (int) floor( longueurs[i] * .1 ) ;
			new_deb		= debut + ten_prct ;
			new_fin		= fin - ten_prct ;
			ecarts[i]	= calcul_std( tableau, new_deb, new_fin );
		}
		else{
			ecarts[i]	= 0 ;
		}
	}

	// PHASE 2 RECHERCHE DES ECARTS MINS POUR CHAQUE INTERVALLE CENTRE SUR CHAQUE SEGMENT	
	for ( i = 0 ; i < nbseg ; i++ ){
		if ( longueurs[i] >= intervalle ){
			min_intervalle[i] = ecarts[i]; 
		}
		else { 
			j = i;	
			while ((j >= 0) && ((segments[i] - longueurs[i] - segments[j]) <= intervalle)){ 
				if ((ecarts[j]<min_intervalle[i]) && (ecarts[j] > 1e-3)) { // initialement ˆ 1, modifiŽ par BB
					min_intervalle[i] = ecarts[j];
				}
				j--;			
			}
			// Recherche avant
			j = i+1;
			while ( ( j<nbseg ) && ( abs( segments[j-1] - segments[i] ) <= intervalle)){ 
				if ((ecarts[j]<min_intervalle[i]) && (ecarts[j] > 1e-3)) { // initialement ˆ 1, modifiŽ par BB					
					min_intervalle[i] = ecarts[j];
				}
				j++;
			}
		}
	}
	
	// PHASE 3 : ETIQUETAGE INDIVIDUEL DES SEGMENTS
	for ( i = 0 ; i < nbseg ; i++ ){
		seuil = min_intervalle[i] * Facteur + 1e-15;
		if (seuil  > ecarts[i]){
			if (longueurs[i]> PauseMin * F_KHZ){
				// Pause
				etiquettes[i] = -1;	
			}
			else{
				// Silence court
				etiquettes[i] = 0;
			}
		}			
		else{
			// Parole
			etiquettes[i] = 1;			
		}		
	}

		
	// PHASE 4 : POST-TRAITEMENTS
	// Mise à -1 des segments extrémités s'ils sont à zéro
	if (etiquettes[nbseg-1] == 0){
		etiquettes[nbseg-1] = -1;
	}
	
	if (etiquettes[0] == 0){
		etiquettes[0] = -1;
	}
	
	// Mise a zero des segments de parole isoles
	for ( i=1 ; i<nbseg-1 ; i++ ){
		if ( (etiquettes[i] == 1) && ( etiquettes[ i - 1 ] ==  -1 ) && (etiquettes[ i + 1 ] == -1 ) ){
			etiquettes[i] =0;
		}		
	}
	

	// Mise a -1 des segments de silences adjacents a un silence long ou
	// dont la longueur cumulee est "longue"
	longueur_cumulee = 0;
	for (i=0;i<nbseg;i++){
		if (etiquettes[i] == 0){
			if ( (i>1) && (etiquettes[i-1] == -1)){
				etiquettes[i] = -1;
			}
			
			if (longueur_cumulee == 0){
				longueur_cumulee = longueurs[i];
			}
			
			if (etiquettes[i+1] == -1){
				// Retour arriere
				j = i;
				while ((j>0) && (etiquettes[j] == 0)){
					etiquettes[j] = -1;
					j--;					
				}				
			}
			else{
				if (etiquettes[i+1] == 0){
					longueur_cumulee = longueur_cumulee + longueurs[i+1];
					if (longueur_cumulee > PauseMin * F_KHZ){
						// Retour arriere
						j = i;
						while ((j>0) && (etiquettes[j] == 0)){
							etiquettes[j] = -1;
							j--;							
						}						
						longueur_cumulee = 0;						
					}					
				}				
			}			
		}
	}

	

	// 2eme passe de Mise a zero des segments de parole isoles (car certains 0 sont passés -1)

	for ( i=1 ; i < nbseg-1 ; i++ ){
		if ( ( etiquettes[i-1] == -1) && ( etiquettes[i+1] == -1 ) ) {
			etiquettes[i] =-1;
		}
		// changement : qlq soit le segment i, si {-1 x -1} alors x => -1
		if (( (etiquettes[i-1] == -1) || ( etiquettes[i+1] == -1)) && (etiquettes[i] ==0) ){
			etiquettes[i] = -1;
		}
		// changement : {-1 0} et {0 -1} => {-1 -1}		
	}
	

	if (verbose){
		for (i=0; i< nbseg; i++){		
			printf("etiquette %i: %i \n",i,  etiquettes[i]);
		}
	
		puts("Speech activity detection achieved");
	}
	return etiquettes;
}
	
