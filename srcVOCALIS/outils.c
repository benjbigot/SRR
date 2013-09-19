/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**                                                                     **/
/**     Date de Creation :              06/02/96                        **/
/**     Date de derniere modif :        08/03/96                        **/
/**     Systeme : Unix                                                  **/
/**     Langage : C                                                     **/
/**     Auteur : FP                                                     **/
/**---------------------------------------------------------------------**/
/**     Description :   Fonctions de gestions des donnees               **/
/**                     et des structures complexes                     **/
/**---------------------------------------------------------------------**/
/**     Journal des versions :                                          **/
/**                                                                     **/
/**             Date    |       Remarques                               **/
/**     ----------------|-----------------------------------------------**/
/**     06/02/96        |       v0                                      **/
/**                     |                                               **/
/**---------------------------------------------------------------------**/
/**     Liste des fonctions :                                           **/
/**                             - copie_une_voyelle                     **/
/**                             - copie_voyelles                        **/
/**                             - creer_une_voyelle                     **/
/**                             - creer_chainon_voyelle                 **/
/**                             - maj_liste_voyelles                    **/
/**                             - initialiser_liste_voyelles            **/
/**                             - creer_cpt_prononc                     **/
/**                             - maj_cpt_prononc                       **/
/**                             - ajouter_cpt_prononc                   **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/


//#include "../Include/systeme.h"
//#include "../Include/macros.h"
#include "systeme.h"
#include "macros.h"



/**************************************************************************/
/***	DECLARATION DES VARIABLES GLOBALES			                    ***/
/**************************************************************************/
int LARGEUR_FENETRE;
/********************************************************/
/*              PROCEDURE calculer_duree_silence        */
/********************************************************/
extern int calculer_duree_silence(int *Tab_seg,int *Tab_sil, int Nb_seg){	
	int nb_fenetres_sil, i, recouvrement;
	float duree_sil;	
	duree_sil = 0;

	for (i=0;i<Nb_seg;i++){
		if (Tab_sil[i]==-1)	{
			duree_sil += (i==0?Tab_seg[i]-1:Tab_seg[i]-Tab_seg[i-1]);
		}
	}
	
	recouvrement = (int) (LARGEUR_FENETRE*FACTEUR_RECOUVREMENT);
	nb_fenetres_sil = (int) (floor((double) (duree_sil/(LARGEUR_FENETRE-recouvrement)) ))-1;
    return nb_fenetres_sil;	
}


/********************************************************/
/*              PROCEDURE copie_une_voyelle             */
/********************************************************/
/*extern void copie_une_voyelle( FILE * Fd,
							  VOYELLE * Pt_voyelle)
	{
    int i,compt;
	
    fprintf( Fd , "%d %d %d %d %d %lf %d %s",
		Pt_voyelle->no_voyelle,
		Pt_voyelle->no_segment,
		Pt_voyelle->debut_seg,
		Pt_voyelle->fin_seg,
		Pt_voyelle->debut_max,
		Pt_voyelle->max_ds,
		Pt_voyelle->confirm,
		Pt_voyelle->label) ;
    fprintf( Fd , "\n" ) ;
	
    compt = 0 ;
    for( i = 1 ; i <= NB_COEFS_VOY ; i ++ ) 
		{
		compt++ ;
		if( compt > 10 )
			{
			fprintf( Fd , "\n" ) ;
			compt = 1 ;
			}
		fprintf( Fd , "%13.5e ", Pt_voyelle->coefs[i] ) ;
		}
    fprintf(Fd, "\n");
	
    return;
	}
*/


/********************************************************/
/*              PROCEDURE copie_voyelles           */
/********************************************************/

/*extern double * copie_voyelles( LISTE_VOYELLES * Pt_liste_voyelles, int nb_voyelles){
	double * sortieVoyelles = (double *) malloc(sizeof(double) * (nb_voyelles+1)) ;
	LISTE_VOYELLES * pt_voyelle_courante;
    int i;
	int no_voyelle_precedente;
	int position;
    no_voyelle_precedente = 0;
    pt_voyelle_courante = Pt_liste_voyelles->suivant;
	i = 0 ;
    while (pt_voyelle_courante != NULL)	{
		if (no_voyelle_precedente != pt_voyelle_courante->pt_voyelle->no_voyelle){
			
			//	    fprintf( Fd , "%d %d %d %d %d %lf %d %s",
			//pt_voyelle_courante->pt_voyelle->no_voyelle,
			//pt_voyelle_courante->pt_voyelle->no_segment,
			//pt_voyelle_courante->pt_voyelle->debut_seg,
			//pt_voyelle_courante->pt_voyelle->fin_seg,
			//pt_voyelle_courante->pt_voyelle->debut_max,
			//pt_voyelle_courante->pt_voyelle->max_ds,
			//pt_voyelle_courante->pt_voyelle->confirm,
			//pt_voyelle_courante->pt_voyelle->label) ;
			//fprintf( Fd , "\n" ) ;
			
			 // compt = 0 ;
			  
			//	for( i = 1 ; i <= NB_COEFS_VOY ; i ++ ) 
			//	{
		//		compt++ ;
	//			if( compt > 10 )
	//			{
	//			fprintf( Fd , "\n" ) ;
	//			compt = 1 ;
	//			}
	//			fprintf( Fd , "%13.5e ", pt_voyelle_courante->pt_voyelle->coefs[i] ) ;
	//			}
	//			fprintf(Fd, "\n");
	//		
			
			
			//printf("%i\n",pt_voyelle_courante->pt_voyelle->confirm);
			if (pt_voyelle_courante->pt_voyelle->confirm == 1){
				//position = (int) floor(Frate * (double) (pt_voyelle_courante->pt_voyelle->debut_max));
				//position = (int) floor((double) (pt_voyelle_courante->pt_voyelle->debut_max));
				position = pt_voyelle_courante->pt_voyelle->debut_max ;
				//fprintf( Fd , "%d\n", position);
				sortieVoyelles[1] = 10;
				printf("%i\n", position);
			}
			no_voyelle_precedente = pt_voyelle_courante->pt_voyelle->no_voyelle;
		}
		pt_voyelle_courante = pt_voyelle_courante->suivant;
	}
	//fprintf(Fd, "\n");
	//printf("\n");
	printf("sortie : %d\n", sortieVoyelles[1]);
	//int uu;for (uu =0;uu<= 10; uu++){printf("%d\n", sortieVoyelles[uu]);}

    return  sortieVoyelles;	
}
/*

/********************************************************/
/*              PROCEDURE copie_voyelles2                */
/********************************************************/
/*extern void copie_voyelles2( FILE * Fd,
							LISTE_VOYELLES * Pt_liste_voyelles,
							double Frate, float F_KHZ)
	{
	
	int duree;
	LISTE_VOYELLES * pt_voyelle_courante;
	int i,compt, no_segment_precedent, position;
	no_segment_precedent = 0;
	pt_voyelle_courante = Pt_liste_voyelles->suivant;
	while (pt_voyelle_courante != NULL)
		{
		if ((no_segment_precedent != pt_voyelle_courante->pt_voyelle->no_segment) &&
			(pt_voyelle_courante->pt_voyelle->confirm == 1))
			{
			duree = pt_voyelle_courante->pt_voyelle->fin_seg - pt_voyelle_courante->pt_voyelle->debut_seg;
			duree = (int) ((double) duree / F_KHZ);
			compt = 0 ;
			for( i = 1 ; i <= 19 ; i ++ ) 
				{
				compt++ ;
				if( compt > 10 )
					{
					fprintf( Fd , "\n" ) ;
					compt = 0;
					}
				if (i < 19) 
					{
					fprintf( Fd , "%13.5e ", pt_voyelle_courante->pt_voyelle->coefs[i] ) ;
					}
				else
					{
					if (i == 19) 
						{
						fprintf( Fd , "%13.5e ", (double) duree) ;
						}
					}
				}
			fprintf(Fd, "\n");
			
			no_segment_precedent = pt_voyelle_courante->pt_voyelle->no_segment;
			}
		pt_voyelle_courante = pt_voyelle_courante->suivant;
		
		}
	fprintf(Fd, "\n");
	return;
	}
*/
/**********************************************************/
/*              PROCEDURE copie_parametres                */
/**********************************************************/
/*extern void copie_parametres( FILE * Fd,
							 float * Coefs, int Nb_lignes, float F_KHZ)
	{
    int i,no,compt;
	
    for(no=0;no<Nb_lignes;no++)
		{
		compt = 0 ;
		
		for( i = 1 ; i <= NB_COEFS; i ++ ) 
			{
			compt++ ;
			if( compt > 10 )
				{
				fprintf( Fd , "\n" ) ;
				compt = 0 ;
				}
			if (i < 19)
				{
				fprintf( Fd , "%13.5e ", Coefs[no*(NB_COEFS+1)+i]);
				}
			else
				{
				if (i == 19) 
					{
					fprintf( Fd , "%13.5e ", (double) (Coefs[no*(NB_COEFS+1)+i]/F_KHZ)) ;
					}
				}
			}
		fprintf(Fd, "\n");
		}
    return;
	}
*/
/********************************************************/
/*              PROCEDURE creer_une_voyelle             */
/********************************************************/
extern VOYELLE * creer_une_voyelle(int No_voyelle){
    VOYELLE *   pt_voyelle;
    int         i;

    if ( (pt_voyelle = new_voyelle()) != NULL){
		pt_voyelle->no_voyelle  = No_voyelle;
		pt_voyelle->no_segment  = 0;
		pt_voyelle->debut_seg   = 0;
		pt_voyelle->fin_seg     = 0;
		pt_voyelle->debut_max   = 0;
		pt_voyelle->max_ds      = 0;
		pt_voyelle->confirm     = 0;
		strcpy(pt_voyelle->label,".....");
		// 24 coefficient comme defini dans global.h
		for (i=0;i<=NB_COEFS_VOY;i++){
			pt_voyelle->coefs[i]=0;
		}
	}
	return (pt_voyelle);	
}

/********************************************************/
/*              PROCEDURE creer_chainon_voyelle         */
/********************************************************/      
extern LISTE_VOYELLES * creer_chainon_voyelle(VOYELLE *Pt_voyelle){
	LISTE_VOYELLES * pt_chainon_voyelle;	
    if ((pt_chainon_voyelle = new_chainage_voyelle())!= NULL){
		strcpy(pt_chainon_voyelle->nom, "XXXX");
		strcpy(pt_chainon_voyelle->format, "XXX");
		pt_chainon_voyelle->no_mot = 0;
		pt_chainon_voyelle->pt_voyelle = Pt_voyelle;
		pt_chainon_voyelle->suivant = NULL;
		pt_chainon_voyelle->dernier = NULL;
		
	}
    return (pt_chainon_voyelle);	
}

/********************************************************/
/*              PROCEDURE maj_liste_voyelles            */
/********************************************************/
extern int maj_liste_voyelles(LISTE_VOYELLES ** Liste_voyelles,  VOYELLE * Pt_voyelle,  int No_voyelle){	
    LISTE_VOYELLES * pt_chainon_voyelle; // le chainon courant créé avec Pt_voyelle	
    if ((pt_chainon_voyelle = creer_chainon_voyelle(Pt_voyelle))!=NULL){
		if ((*Liste_voyelles)->dernier == NULL){
			(*Liste_voyelles)->suivant = pt_chainon_voyelle;
			(*Liste_voyelles)->dernier = pt_chainon_voyelle;
			pt_chainon_voyelle->pt_voyelle->no_voyelle = No_voyelle;
			return (0);		
		}
		else{			
			(*Liste_voyelles)->dernier->suivant = pt_chainon_voyelle;
			(*Liste_voyelles)->dernier = pt_chainon_voyelle;
			pt_chainon_voyelle->pt_voyelle->no_voyelle = No_voyelle;
			return (0);			
		}
	}
    else{
		puts("erreur de mise a jour de la lite voyelle");
		return (1);
	}
}

/********************************************************/
/*      PROCEDURE initialiser_liste_voyelles2            */
/********************************************************/
extern int initialiser_liste_voyelles2(char * Format,int No_mot, LISTE_VOYELLES ** Pt_liste_voyelles){
	if (((*Pt_liste_voyelles) = creer_chainon_voyelle(NULL)) == NULL){
		printf("Echec de l'allocation de la liste de voyelles du mot no %d\n", No_mot);
		return (1);
	}
    else{		
		strcpy((*Pt_liste_voyelles)->format, Format);
		(*Pt_liste_voyelles)->no_mot = No_mot;
		return (0);
	}	
}

/********************************************************/
/*      PROCEDURE Free_liste_voyelles                   */
/********************************************************/
extern int free_liste_voyelles(LISTE_VOYELLES ** Liste_voyelles)
	{
    LISTE_VOYELLES * pt_chainon_voyelle;
    int i, nb_voyelles;
	long int *adresses;
	if ((*Liste_voyelles)->dernier == NULL)
		{
		// Il s'agit du premier chainon 
		free((char *) *Liste_voyelles);
		}
	else
		{
		// Parcours du chainage pour obtenir les adresses des chainons
		pt_chainon_voyelle = *Liste_voyelles;
		nb_voyelles = pt_chainon_voyelle->dernier->pt_voyelle->no_voyelle;
		adresses = (long int *) malloc (nb_voyelles*sizeof(pt_chainon_voyelle));
			
		for (i=0;i<nb_voyelles;i++)
			{
			adresses[i] = (long int) pt_chainon_voyelle;
			pt_chainon_voyelle = pt_chainon_voyelle->suivant;
			}
		// Liberation des chainons
		for (i=0;i<nb_voyelles;i++)
			{
			free((char *) adresses[i]);
			}
		}
	return (0);
	}
