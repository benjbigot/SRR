/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**                                                                     **/
/** Date de Creation :      avant                                       **/
/** Date de derniere modif :    10/11/95                                **/
/** Systeme : Unix                                                      **/
/** Langage : C                                                         **/
/** Auteur : FP                                                         **/
/**---------------------------------------------------------------------**/
/** Description :   procedures de calculs des coefs cepstraux           **/
/**         pour chaque segment                                         **/
/**---------------------------------------------------------------------**/
/** Journal des versions :                                              **/
/**                                                                     **/
/**     Date    |   Remarques                                           **/
/** ----------------|---------------------------------------------------**/
/** 10/11/95    |   v0                                                  **/
/**         |                                                           **/
/**         |                                                           **/
/**---------------------------------------------------------------------**/
/** Liste des fonctions :                                               **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/


//#include "../Include/systeme.h"
#include "systeme.h"

extern int LARGEUR_FENETRE;
extern FILE * fd_err;


/********************************************************/
/*		PROCEDURE ener_bruit		        */
/********************************************************/
void ener_bruit (int p, int tsig[], float tbruit[], float *ebruit)   /* Calcule les energies sur 256 ech. */
	{
	float x[1025];	
	int i,pfin,px = 1;
	int tb = 256;
	for (i=0; i<34; i++) { tbruit[i] = 0.; }
	for (i=0; i<1024; i++) { x[i] = 0.; }
	pfin = p+256;
	while (p < pfin)
		{
		x[px++] = (float) tsig[p++];
		}
	x[0] = 0.;		
	CINIT(tb);
	for (i=1; i <= tb; i++) { x[i] = x[i+1]-(0.9 * x[i]); }
	x[tb+1] = 0.;
	x[tb+2] = 0.;
	i = FENSIG(x,tb);			
	CFILT (tb,x,tbruit,ebruit);
	tbruit[0] = 0.;
	return;
	}

/********************************************************/
/*	PROCEDURE calcul_cepstraux_un_seg	        */
/********************************************************/
//extern void calcul_cepstraux_un_seg(int taille_seg , int Segment[2], int tableau_sig[], int taille_sig, float tableau_cepst[]) 
extern void calcul_cepstraux_un_seg(int taille_seg , int Segment[2], float tableau_sig[], int taille_sig, float tableau_cepst[]){
	//puts("ok2");
    int i,idur,lmil,lpas, deb, fin, nlong, newdeb;
    int  nlongmax;
    float       tableau_cepst1[21] ;
    float       tableau_cepst2[21] ;
    float       tableau_cepst3[21] ;
    float       tableau_cepst4[21] ;
    float       tableau_cepst5[21] ;
    float       regtableau_cepst[21] ;
	
    
    float x[1025], cc;
	
	// Ajout d'un test: si la duree du segment est nul => on met les parametres à 0
	
	
	if (Segment[0]==Segment[1])	{
		for (i=0 ; i<=NB_COEFS+1 ; i=i+1) tableau_cepst[i] = 0;
		
	}
	else{
		
		nlongmax=256;		
		
		/* Recopie du nombre de segments */
		taille_seg = taille_seg -1;
		
		/*  Calcul sur UN SEGMENT */				
		deb =Segment[0];
		fin = Segment[1];
				
		idur=fin-deb;
		lmil = (fin + deb)/2;
		nlong =nlongmax;				
		
		lpas = nlong /2;
		
		newdeb = lmil - 3*lpas;
		
		/*   les bornes du segment a analyser sont newdeb,fin de longueur idur:	    */
		/*   il s'agit donc de calculer une regression des coefficients             */
		/*   sur cinq fenetres, l'ensemble est centre si c'est possible sur le segment*/
		
		/*    deux cas particuliers peuvent se presenter :                          */
		/*       une au moins des bornes de la premiere fenetre est inferieure a 0  */
		if (newdeb < 0) newdeb = 0;
		/*       une au moins des bornes de la derniere fenetre est superieure a fin */
		if((newdeb+6*(lpas+1)) > taille_sig) newdeb = taille_sig - 6*(lpas+1);
		
				
		/* analyse de la premiere fenetre                                            */
		lpas = nlong /2;
		for (i=1; i <= (nlong +1); i=i+1)
			x[i]= (float) tableau_sig[newdeb +i];
		for ( i=1; i <= 20; i=i+1)
			tableau_cepst1[i]=0.;
		for ( i=(nlong+2) ; i <= 1024; i=i+1)
			x[i]=0.;
		
		paramlev(nlong,x,tableau_cepst1,NULL); 
		
		
		
		/* analyse de la deuxieme fenetre                                            */
		newdeb = newdeb + lpas;
		for (i=1; i <= (nlong +1); i=i+1)
			x[i]= (float) tableau_sig[newdeb +i];
		for ( i=1; i <= 20; i=i+1)
			tableau_cepst2[i]=0.;
		for ( i=(nlong+2) ; i <= 1024; i=i+1)
			x[i]=0.;
		paramlev(nlong,x,tableau_cepst2, NULL); 
		
		
		/* analyse de la troisieme fenetre                                            */
		newdeb = newdeb + lpas;
		for (i=1; i <= (nlong +1); i=i+1)
			x[i]= (float) tableau_sig[newdeb +i];
		for ( i=1; i <= 20; i=i+1)
			tableau_cepst3[i]=0.;
		for ( i=(nlong+2) ; i <= 1024; i=i+1)
			x[i]=0.;
		paramlev(nlong,x,tableau_cepst3, NULL); 
		
		/* analyse de la quatrieme fenetre                                            */
		newdeb = newdeb + lpas;
		for (i=1; i <= (nlong +1); i=i+1)
			x[i]= (float) tableau_sig[newdeb +i];
		for ( i=1; i <= 20; i=i+1)
			tableau_cepst4[i]=0.;
		for ( i=(nlong+2) ; i <= 1024; i=i+1)
			x[i]=0.;
		paramlev(nlong,x,tableau_cepst4, NULL); 
		
		/* analyse de la cinquieme fenetre                                            */
		newdeb = newdeb + lpas;
		for (i=1; i <= (nlong +1); i=i+1)
			x[i]= (float) tableau_sig[newdeb +i];
		for ( i=1; i <= 20; i=i+1)
			tableau_cepst5[i]=0.;
		for ( i=(nlong+2) ; i <= 1024; i=i+1)
			x[i]=0.;
		paramlev(nlong,x,tableau_cepst5, NULL); 
		
		/* calcul des coefficients de regression                                    */
		
		for (i=1 ; i <= 9; i=i+1)
			{
			cc = ( -2*tableau_cepst1[i] - tableau_cepst2[i] + tableau_cepst4[i] +
				2 * tableau_cepst5[i] ) / 10.;
			if ( lpas <= 64 ) cc = 2 * cc;
			regtableau_cepst[i] = cc;
			}
		
		
		/*      calcul de la fenetre milieu                                          */
		if (idur > 512) nlong =512;                                          
		lpas =nlong/2;                                                       
		newdeb = lmil - lpas;
		if (newdeb < 0 ) newdeb=0;
		if ((newdeb + 2*lpas) > taille_sig ) newdeb =taille_sig - 2*lpas;
		for (i=1; i <= (nlong +1); i=i+1)
			x[i]= (float) tableau_sig[newdeb +i];
		for ( i=1; i <= 20; i=i+1)
			tableau_cepst[i]=0.;
		for ( i=(nlong+2) ; i <= 1024; i=i+1)
			x[i]=0.;
		paramlev(nlong,x,tableau_cepst, NULL); 
		
		for (i=10 ; ((i <= 18)&&(i<=NB_COEFS)) ; i=i+1)
			tableau_cepst[i] = regtableau_cepst[i-9];
		if (NB_COEFS>=19)
			tableau_cepst[19] = (float) (idur);
		}
		
}
