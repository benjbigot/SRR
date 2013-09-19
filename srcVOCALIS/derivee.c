/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**                                                                     **/
/** Date de Creation :      20/10/95                                    **/
/** Date de derniere modif :    18/03/96                                **/
/** Systeme : Unix                                                      **/
/** Langage : C                                                         **/
/** Auteur : FP                                                         **/
/**---------------------------------------------------------------------**/
/** Description :   procedures de calcul du tableau des                 **/
/**         derivees spectrales par trame de 256 points                 **/
/**                                                                     **/
/**---------------------------------------------------------------------**/
/** Journal des versions :                                              **/
/**                                                                     **/
/**     Date    |   Remarques                                           **/
/** ----------------|---------------------------------------------------**/
/** 20/10/95    |   v0                                                  **/
/**             |                                                       **/
/**             |                                                       **/
/**---------------------------------------------------------------------**/
/** Liste des fonctions :                                               **/
/**                                                                     **/
/**     calculer_energie                                                **/
/**     calculer_energie_seg                                            **/
/**     calculer_critere                                                **/
/**     extraire_segment_trame                                          **/
/*************************************************************************/
/*************************************************************************/
  

//#include "../Include/systeme.h"
#include "systeme.h"


/*************************************************************************/
/***			DECLARATION DES VARIABLES EXTERNES	       ***/
/*************************************************************************/
extern	int FLAG_FILTRES[28];
extern	int FLAG_FORMANT[28];
extern  int fcoup[29];
extern int LARGEUR_FENETRE;
extern float F_HZ;
extern float F_KHZ;



/********************************************************/
/*		PROCEDURE calculer_energie		*/
/********************************************************/
void calculer_energie(float *Signal, int Recouvrement, float *Energie, int Nb_fenetres, float *Energie_max){		
	int			i,j;
	int			fenetre_courante, trame_max;
	float * 	signal;
	fenetre_courante	= 0;
	*Energie_max		= 0;	
	signal	= (float *) malloc( ( LARGEUR_FENETRE + 10 ) * sizeof(float) );
	
	if (signal == NULL){
	    puts("Erreur : allocation de signal foireuse");
	    exit(1);
	}

	for(i=0; fenetre_courante<Nb_fenetres; i += Recouvrement){
		//*--------------------------------------------------------------
		//* signal[0] n'est pas utilise...				
		//* signal[1:256] contient le vrai signal...		 	
		//* signal[257:260] sont des valeurs utilisees comme buffers	
		//*--------------------------------------------------------------
		for (j=0 ; j<=LARGEUR_FENETRE ; j++ ) {
			signal[j] = Signal[j+i];
		}
			
		for(j=(LARGEUR_FENETRE+1); j<(LARGEUR_FENETRE+9); j++){
			signal[j] = 0.0;
		}
		
		//calcul de l'énergie
		paramlev(LARGEUR_FENETRE, signal, NULL,  &Energie[fenetre_courante*(NB_FILTRES+1)]);
		
		if ((Energie[fenetre_courante*(NB_FILTRES+1)]) > *Energie_max){
		    trame_max = fenetre_courante;
		    *Energie_max = Energie[fenetre_courante*(NB_FILTRES+1)];			
		}
		fenetre_courante++;	    
	
	}	
	free( signal);
}

/********************************************************/
/*		PROCEDURE calculer_energie_seg		*/
/********************************************************/
void calculer_energie_seg(int *Signal,/* deja alloue */
			  int debut_seg,
			  int fin_seg,
			  float *Energie)	/* deja alloue */
{
    int		j;
    int		largeur_fenetre;
    
    float * 	signal;
	


    largeur_fenetre = fin_seg - debut_seg;
    
    fin_seg = debut_seg + ((int) (largeur_fenetre/256))*256;
    largeur_fenetre = fin_seg - debut_seg;
    if (largeur_fenetre == 0) largeur_fenetre = 256;
    
	signal = (float *) malloc ((largeur_fenetre+5) * sizeof(float));
	
    if (signal == NULL)
    {
	exit(-1);
    }

    for(j=debut_seg; j<=fin_seg; j++)
    {
	signal[j] = (float) Signal[j];
    }
		
    for(j=(largeur_fenetre+1); j<(largeur_fenetre+4); j++)
	signal[j] = 0.0;
    paramlev(largeur_fenetre, signal, NULL, Energie);

    free( (char *) signal);

}

/********************************************************/
/*		PROCEDURE calculer_critere		*/
/********************************************************/
void calculer_critere(float *Energie, float *Derivee_spectrale, float *Alpha, float *Beta, int Nb_fenetres, int Nb_fenetres_silence){

    int		i,j,nb_filtres_valides, jmin, jmax;
    int		nb_filtres_F1, nb_filtres_F2;
    
    float	*delta;
    float	*moyenne;
    float	total;
    float	*derivee;
    float	valeur_filtre;
    float	valeur_totale;
    float	valeur_BF;
    float	temp;
    
	delta	= (float *) malloc((Nb_fenetres+1) * sizeof(float));
	moyenne = (float *) malloc((Nb_fenetres+1) * sizeof(float));
	derivee = (float *) malloc((Nb_fenetres+1) * sizeof(float));
	
	derivee[0] = 0;    
    for(i=1; i<= Nb_fenetres; i++){
		total = 0;
		nb_filtres_valides = 0;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
		/* Calcul de la moyenne du Spectre */
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
		for (j=1; j<= NB_FILTRES; j++){
			if (FLAG_FILTRES[j]!=0){
				nb_filtres_valides++;
				total += (Energie[(i-1)*(NB_FILTRES+1)+j]); //calcul moyenne sur fenetre court terme
			}
		}
		moyenne[i] = (float) (total / nb_filtres_valides) ;
		
		derivee[i] = 0;
		valeur_totale = 0;
		valeur_BF = 0;
		Alpha[i] = 0;
		Beta[i] = 0;
		temp = 0;
		
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
		/* Calcul du Critere d'Aire Spectrale */
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
		for (j=1; j<= NB_FILTRES; j++){
			valeur_filtre = ( Energie[(i-1)*(NB_FILTRES+1) + j ] - moyenne[i]);
			if ((FLAG_FILTRES[j] != 0) && (valeur_filtre > 0)){
				derivee[i] += fabs((float) valeur_filtre);
				valeur_totale += valeur_filtre;
				
				/* Calcul pour Alpha */
				if (FLAG_FILTRES[j] == 2){
					valeur_BF += valeur_filtre;
				}	
			}	
		}
		Alpha[i] = (valeur_BF/(valeur_totale+1e-10));
		derivee[i] *= Alpha[i];
		derivee[0] += derivee[i];
	}    

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
    /* Filtrage de l'Aire Spectrale */
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
    Derivee_spectrale[1] = 0.5*derivee[1] + 0.5*derivee[2]; 
    Derivee_spectrale[Nb_fenetres] = 0.5*derivee[Nb_fenetres] + 0.5*derivee[Nb_fenetres-1]; 
	
    for(i=2; i< Nb_fenetres; i++){
		Derivee_spectrale[i] = 0.33 * derivee[i-1] + 0.33 * derivee[i] + 0.33 * derivee[i+1];	
	}

    Derivee_spectrale[0]= derivee[0]/(Nb_fenetres-Nb_fenetres_silence);
	free(  delta);	
	free(  moyenne);	
	free(  derivee);
}


/********************************************************/
/*		PROCEDURE calculer_critere5		*/
/********************************************************/
// Version avec filtrage sur 5 trames
/*void calculer_critere5(float *Energie,			// deja alloue 
					double *Derivee_spectrale, 	// deja alloue 
					float *Alpha,				// deja alloue 
					float *Beta,				// deja alloue 
					int Nb_fenetres,
					int Nb_fenetres_silence)
{
    int		 i,j,nb_filtres_valides, jmin, jmax;
    int		nb_filtres_F1, nb_filtres_F2;
    
    double	*delta;
    float	*moyenne;
    double	total;
    double	*derivee;
    float	valeur_filtre;
    float	valeur_totale;
    float	valeur_BF;
    float temp;

    delta = (double *) malloc((Nb_fenetres+1) * sizeof(double));
    moyenne = (float *) malloc((Nb_fenetres+1) * sizeof(float));
    derivee = (double *) malloc((Nb_fenetres+1) * sizeof(double));
    
	
	
    derivee[0] = 0;
    
    for(i=1; i<= Nb_fenetres; i++)
    {
	total = 0;
	nb_filtres_valides = 0;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	// Calcul de la moyenne du Spectre 
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
	for (j=1; j<= NB_FILTRES; j++)
	{
	    if (FLAG_FILTRES[j]!=0)
	    {
		nb_filtres_valides++;
		total += (Energie[(i-1)*(NB_FILTRES+1)+j]);
	    }
	}
	moyenne[i] = (float) (total / nb_filtres_valides) ;

	derivee[i] = 0;
	valeur_totale = 0;
	valeur_BF = 0;
	Alpha[i] = 0;
	Beta[i] = 0;
	temp = 0;
	
	//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
	//* Calcul du Critere d'Aire Spectrale //
	//*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
	for (j=1; j<= NB_FILTRES; j++)
	{
	  valeur_filtre = (Energie[(i-1)*(NB_FILTRES+1)+j]- moyenne[i]);
	  if ((FLAG_FILTRES[j] != 0) && (valeur_filtre > 0))
	    {
	      //*printf("Filtre OK\t FORMANT = %d\t",FLAG_FORMANT[j]);
	      derivee[i] += fabs((double) valeur_filtre);
	      valeur_totale += valeur_filtre;

	      //* Calcul pour Alpha 
	      if (FLAG_FILTRES[j] == 2)
		valeur_BF += valeur_filtre;

	    }
	}
	Alpha[i] = (valeur_BF/valeur_totale);
	//fprintf(fd_err, "Alpha de %d = %f\n",i, Alpha[i]);
	derivee[i] *= Alpha[i];
	derivee[0] += derivee[i];
	//Derivee_spectrale[i] = derivee[i];
    }
    //*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//	
    //* Filtrage de l'Aire Spectrale //
    //*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//    Derivee_spectrale[1] = 0.5*derivee[1] + 0.5*derivee[2]; 
//    Derivee_spectrale[Nb_fenetres] = 0.5*derivee[Nb_fenetres] + 0.5*derivee[Nb_fenetres-1]; 
//    for(i=2; i< Nb_fenetres; i++)
//      {
//		Derivee_spectrale[i] = 0.33 * derivee[i-1] + 0.33 * derivee[i] + 0.33 * derivee[i+1];
//	 }
    for(i=1; i<Nb_fenetres; i++)
      {
		Derivee_spectrale[i] = 
		derivee[(((i-2)>0)?(i-2):1)] +
		derivee[(((i-1)>0)?(i-1):1)] +
		derivee[i] +
		derivee[(((i+1)<=Nb_fenetres)?(i+1):Nb_fenetres)] +
		derivee[(((i+2)<=Nb_fenetres)?(i+2):Nb_fenetres)] ;
		Derivee_spectrale[i] /= 5;
      }

    Derivee_spectrale[0]= derivee[0]/(Nb_fenetres-Nb_fenetres_silence);
    free( (char *) delta);
    free( (char *) moyenne);
    free( (char *) derivee);
}

*/
/************************************************/
/*		PROCEDURE extraire_segment_trame     	*/
/************************************************/
/*int extraire_segment_trame(int No_trame, int *Tab_seg, int Nb_seg,
			   float *Energie_segment, int *Signal)

{

    int    i;
    int trame;
    int trouve;
    int debut_seg;
    int fin_seg;
    float *energie;
    
    
    i = 0;
 
    //*--------------------------------------------------------------
    //*     Passage de la position de la trame en echantillons     
    //*--------------------------------------------------------------
    trame = (int) (No_trame * LARGEUR_FENETRE * FACTEUR_RECOUVREMENT);
    
    if(trame >= Tab_seg[Nb_seg-1])
    trouve = 0;
    
    while((i <(Nb_seg-1)) && (!trouve))
    {
	
	if ((Tab_seg[i] <= trame) &&
	    (trame < Tab_seg[i+1]))
	{
	    if((Tab_seg[i] == trame) && (i!=0))
	    {
		debut_seg=Tab_seg[i-1];
		fin_seg=Tab_seg[i+1];
		trouve = 1;
	    }
	    else
	    {
		debut_seg=Tab_seg[i];
		fin_seg=Tab_seg[i+1];
		trouve = 1;
	    }
	}
	i++;
    }
    energie = (float *) malloc ((NB_FILTRES+1) * sizeof(float));
	
    if (energie == NULL)
    {
	exit(-1);
    }
    calculer_energie_seg(Signal, debut_seg, fin_seg, energie);
    *Energie_segment = energie[0];
    free ( (char *) energie);
    return (0);
    
}
*/
