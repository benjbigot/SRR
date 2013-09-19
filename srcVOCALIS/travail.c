/****************************************************************************************/
/*											*/
/* 	Fichier contenant les fonctions de travail de l'application Telephone		*/
/*	--> Markov / Viterbi sur 500 mots dits par t‰l‰phone provenant du CNET 		*/
/*											*/
/****************************************************************************************/

//#include "../Include/systeme.h"
//#include "../Include/tel.h"
#include "systeme.h"
#include "tel.h"

extern float F_HZ;
extern float F_KHZ;

//===================================================================
// Fonction de chargement du signal d'un mots dans un tableau 
/*extern void charge_segments( FILE *fd_seg ,  long pos_debut_seg , int taille_seg , int tableau_seg[] ) 
{
int compt ;

	fseek( fd_seg , pos_debut_seg , 0 ) ;

	compt = 0 ;
	while( (!feof( fd_seg ))  && ( compt < taille_seg ) )
	{
		fscanf( fd_seg , "%d" , &tableau_seg[compt] ) ;
		compt ++ ;
	}

	return ;
}
*/
//===================================================================
/*extern void charge_segments2( FILE *fd_seg ,  long pos_debut_seg , int *taille_seg , int tableau_seg[] ) 
{
    int compt ;

    fseek( fd_seg , pos_debut_seg , 0 ) ;
    fscanf(fd_seg,"%*d");
    
    compt = 0 ;
    while( (!feof( fd_seg ))  && ( compt < *taille_seg ) )
    {
	fscanf( fd_seg , "%d %*s" , &tableau_seg[compt] ) ;
	compt ++ ;
    }
    *taille_seg = compt-1;
    
    return ;
}
*/
/*________________________________________________________________________________________________*/
/*
extern void charge_segments3( FILE *fd_seg, int *taille_seg , int tableau_seg[], double Frate) 
{
    int compt ;
    compt = 0 ;
    while( (!feof( fd_seg ))  && ( compt < *taille_seg ) )
    {
	fscanf( fd_seg , "%d" , &tableau_seg[compt]);
	if (fabs(Frate - 1.0) > 1e-5)
		tableau_seg[compt] = (int) (tableau_seg[compt]/Frate);
//	fprintf(fd, "Segment %d\n", tableau_seg[compt]);
	compt ++ ;
    }
    *taille_seg = compt-1;
    //fclose(fd);
    return ;
}
 */
/*________________________________________________________________________________________________*/
/*
extern void charge_silence( FILE *fd_sil, int taille_seg , int tableau_sil[]) 
{
    int compt ;
    compt = 0 ;
    while( (!feof( fd_sil ))  && ( compt < taille_seg ) )
	    {
		fscanf( fd_sil , "%d" , &tableau_sil[compt]);
		compt ++ ;
		}
    return ;
}
 */
/*________________________________________________________________________________________________*/
/*
extern void charge_f0( FILE *fd_f0, int *nbF0, double *timef0, double *f0) 
{
    int compt, NbMax ;
	char bid[256];
    compt = 0 ;
	NbMax = (int) f0[0]; // Tableau alloué de 0 à NbMax
	// On saute les 2 lignes entete
	fgets(bid,200,fd_f0);
	fgets(bid,200,fd_f0);
	// On charge les valeurs
    while(!feof(fd_f0) && compt<NbMax)
	    {
		fscanf( fd_f0 , "%lf\t%lf" , &timef0[compt], &f0[compt]);
		compt ++ ;
		}
	*nbF0 = compt - 1;
    return ;
}
 */
//___________________________________________________________________________
/*
extern void charge_reels( FILE *fd_seg , long  pos_debut_seg , int taille_seg ,float tableau_seg[] ) 
{
int compt ;

	fseek( fd_seg , pos_debut_seg , 0 ) ;

	compt = 0 ;
	while( (!feof( fd_seg ))  && ( compt < taille_seg ) )
	{
		fscanf( fd_seg , "%f" , &tableau_seg[compt] ) ;
		compt ++ ;
	}

	return ;
}

extern void charge_double( FILE *fd_seg ,  long pos_debut_seg ,int  taille_seg , double tableau_seg[] ) 
{
int compt ;

	fseek( fd_seg , pos_debut_seg , 0 ) ;

	compt = 0 ;
	while( (!feof( fd_seg ))  && ( compt < taille_seg ) )
	{
		fscanf( fd_seg , "%lf" , &tableau_seg[compt] ) ;
		compt ++ ;
	}

	return ;
}
*/
//=========================================================
// Fonction de lecture d'une chaine de caracteres 
/*
extern void lecture_chaine(  char *mot ,int  max  ,FILE *  fd )
{ 
int i ;

		fgets( mot , max , fd  ) ;

		i = 0 ;
		while( (mot[i] != '\n') && (mot[i] != '\0') && ( i < max ) ) 
			i++ ;

		if( mot[i] == '\n' ) 
			mot[i] = '\0' ;

		return ;
}
*/
//===========================================================================
//Fonction de décision voiced/unvoiced - vote majoritaire sur un segment

extern int isvoiced(int *segment_voyelle, float *timeF0, float *f0, int nbF0){
	float debut;
	float fin;
	int nbV, nbUV, i;
	i = 0;
	nbV = 0;
	nbUV = 0;
		debut = (float)segment_voyelle[0]/ F_HZ;
	fin = (float)segment_voyelle[1]/  F_HZ;
	
	// Recherche du premier timeF0 du segment	
	while ((timeF0[i]<debut) && (i<nbF0)) {
		i++;
	}
	
	if (i<(nbF0-1)){
		while ((timeF0[i]<fin) && (i<nbF0)){
			if (f0[i] < 1. ){
				nbUV++;			
			}
			else{
				nbV++;
			}
			i++;
		}
		if (nbUV>nbV){
			return 0;
		}
		else{
			return 1;
		}
	}
	else{
		return 0;
	}
}
