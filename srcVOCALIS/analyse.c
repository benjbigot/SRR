/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**                                                                     **/
/**     Date de Creation :              31/10/95                        **/
/**     Date de derniere modif :        15/07/96                        **/
/**     Systeme : Unix                                                  **/
/**     Langage : C                                                     **/
/**     Auteur : SE / FP                                                **/
/**---------------------------------------------------------------------**/
/**     Description :   Fonctions de detection de pulses & voyelles     **/
/**                     dans la parole - Programme de Serge Eichene     **/
/**---------------------------------------------------------------------**/
/**     Journal des versions :                                          **/
/**                                                                     **/
/**             Date    |       Remarques                               **/
/**     ----------------|-----------------------------------------------**/
/**     31/10/95        |       v0  Version de Serge                    **/
/**     31/10/95        |       v1  Version de Serge simplifiee         **/
/**     01/12/95        |           Version ok pour le lint             **/
/**     25/03/96        |       v2  Ajout de la confirmation            **/
/**                     |               de la detection                 **/
/**     01/07/96        |       v3  Nouveau Critere                     **/
/**---------------------------------------------------------------------**/
/**     Liste des fonctions :                                           **/
/**                             - confirmer_une_voyelle                 **/
/**                             - confirmer_voyelles                    **/
/**                             - detecter_voyelles                     **/
/**                             - extraire_sommet	                    **/
/**                             - extraire_segment_voyelle              **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/

//#include "../Include/systeme.h"
#include "systeme.h"
#include "global.h"
#include "vocal.h"

/*************************************************************************/
/***                    DECLARATION DES VARIABLES EXTERNES             ***/
/*************************************************************************/
extern  int FLAG_ENERGIE[27];
extern  FILE * fd_err;
extern float F_HZ;
extern float F_KHZ;
extern int LARGEUR_FENETRE;


/********************************************************/
/*              PROCEDURE confirmer_une_voyelle         */
/********************************************************/
int confirmer_une_voyelle(VOYELLE * Pt_voyelle, float * Aire_moyenne, float * alpha, float * beta){
    int         i,j,debut_seg,fin_seg;
	int			largeur_trame;
    double      energie_bf, energie_totale;  // peut etre a passer en double.

    largeur_trame = (int) ceil(LARGEUR_FENETRE * (1 - FACTEUR_RECOUVREMENT) );
    
    debut_seg   = (int) ceil((double) ((Pt_voyelle->debut_seg)/largeur_trame));
    fin_seg     = (int) floor((double) ((Pt_voyelle->fin_seg)/largeur_trame));
    	
	if ((fin_seg-debut_seg)*largeur_trame < (int) (1.5 * F_HZ / 100) ){
		printf ("Segment %d - %d trop court \n",debut_seg, fin_seg);	
		return (0);		/* segment < 15 ms */
    }

	if ((alpha[Pt_voyelle->debut_max] < 0.10)){
		return (0);      
	}
    else{
		//puts("ok");
		return (1);
    }
}

/********************************************************/
/*              PROCEDURE extraire_sommets              */
/********************************************************/
int extraire_sommets( float *Aire, LISTE_VOYELLES **Liste, int NbTrames, int Seuil){
    float	*derivee;
    float	*extrema;
    int		nb_voyelles;
    int		i;
    VOYELLE	*pt_voyelle;
	derivee  = (float *) malloc(NbTrames * sizeof(float));
    extrema  = (float *) malloc(NbTrames * sizeof(float));
    	
	nb_voyelles = 1;

    for (i=1;i<NbTrames;i++){
		derivee[i] = Aire[i+1] - Aire[i]; // tout en float
	}
	
    for (i=1;i<NbTrames-1;i++){
		extrema[i] = derivee[i+1] * derivee[i];	 // tout en float
	}
	
		
    for ( i=1 ; i<NbTrames-2 ; i++ ){
		// Attention : un Seuil (gnralement 1/10 de l'aire moyenne sur la phrase) est applique !!
		if ((extrema[i] <= 0.0) && (Aire[i+1] > Aire[i]) && (Aire[i+1] > (Aire[0] / Seuil ))){		
			// creation d'une structure voyelle
			if((pt_voyelle = (VOYELLE *) creer_une_voyelle(nb_voyelles))!= NULL){
				pt_voyelle->debut_max = i+1;
				pt_voyelle->max_ds   = Aire[i+1];				
			}

			if (maj_liste_voyelles(Liste, pt_voyelle, nb_voyelles)){				
				printf("MERDE ds extraire_sommets %i \n", nb_voyelles);
				return (1);
			}					
			
			nb_voyelles++;
		}
	}
	free( derivee);
    free( extrema);

    return(0);
}


/********************************************************/
/*              PROCEDURE detecter_voyelles             */
/********************************************************/
char detecter_voyelles(float * Derivee, LISTE_VOYELLES ** Liste_voyelles, int NbTrames, int Seuil){
	int jj;
    if(extraire_sommets(Derivee, Liste_voyelles , NbTrames , Seuil)){
		puts("erreur dans detecter voyelle/extraire sommet");
		return (1);

	}
	return(0);
}


/********************************************************/
/*              PROCEDURE extraire_segment_voyelle      */
/********************************************************/
int extraire_segment_voyelle(VOYELLE *Pt_voyelle, int *Tab_seg,int Nb_seg){
    int    i;
    int voyelle;    
    i = 0;
	

    /*--------------------------------------------------------------*/
    /*     Passage de la position de la voyelle en echantillons     */
    /*--------------------------------------------------------------*/	
    voyelle = (int) (Pt_voyelle->debut_max * LARGEUR_FENETRE * FACTEUR_RECOUVREMENT);
	if(voyelle >=  Tab_seg[Nb_seg-1]){
		return(1);
	}
 

	if (voyelle < Tab_seg[0]){		
		Pt_voyelle->no_segment=0;
		Pt_voyelle->debut_seg= 1;
		Pt_voyelle->fin_seg=Tab_seg[0];
		return(0);
	}	
 
    while(i <(Nb_seg-1)){
		if ((Tab_seg[i] <= voyelle) && (voyelle < Tab_seg[i+1])){
			if((Tab_seg[i] == voyelle) && (i!=0)){
				Pt_voyelle->no_segment=i;
				Pt_voyelle->debut_seg=Tab_seg[i-1];
				Pt_voyelle->fin_seg=Tab_seg[i+1];
				return(0);
			}
			else	    {
				Pt_voyelle->no_segment=i+1;
				Pt_voyelle->debut_seg=Tab_seg[i];
				Pt_voyelle->fin_seg=Tab_seg[i+1];
				return(0);
			}
		}
		i++;
	}
	return(1);
}
