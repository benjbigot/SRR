/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**                                                                     **/
/** Date de Creation :      avant                                       **/
/** Date de derniere modif :    08/11/95                                **/
/** Systeme : Unix                                                      **/
/** Langage : C                                                         **/
/** Auteur : RAO/BJ/JBP/FP                                              **/
/**---------------------------------------------------------------------**/
/** Description :   programme d'analyse du signal par                   **/
/**         recherche des coefficients cepstraux                        **/
/**                                                                     **/
/**---------------------------------------------------------------------**/
/** Journal des versions :                                              **/
/**                                                                     **/
/**     Date    |   Remarques                                           **/
/** ----------------|---------------------------------------------------**/
/** 08/11/95    |   v0  paramlev modifie                                **/
/**             |                                                       **/
/**             |                                                       **/
/**---------------------------------------------------------------------**/
/** Liste des fonctions :                                               **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/


/* variables globales 
*/
extern float F_HZ;
extern float F_KHZ;
extern int LARGEUR_FENETRE;


//#include "../Include/systeme.h"
//#include "../Include/sslcepst.h"
#include "systeme.h"
#include "sslcepst.h"

/********************************************************/
/*		PROCEDURE HAMJEAN   		        */
/********************************************************/
/*  sous-programme d initialisation des tables pour l'analyse   */
/*  cepstrale                                                   */

int HAMJEAN (float fen[],int lgsig)
	{
    int i;
    float cl1,z;
    if ( lgsig < 0) return(0);
    cl1 = 1.;
    larg1 = lgsig/2 + 1;
    l1 = ( lgsig+1) /2;
    clm = cl1 / lgsig * 2.;
    for ( i = 1; i <= l1; i=i+1)
		fen[i] = .54 - .46 * (float)(cos (  (double)(pi*(2*i-1)) / (double)(lgsig)  )  );
    z=0.;
    for ( i=1 ; i <= l1; i=i+1)
		z = z + fen[i];
    z = 2.* z;
    for ( i =1; i <= l1; i=i+1)
		fen[i] = fen[i] / z;	
    return(1);
	}

/********************************************************/
/*		PROCEDURE FENSIG    		        */
/********************************************************/
int FENSIG ( float v[],int lgsig)
	{
    int i;
    for ( i =1; i <= l1; i=i+1)
		{
		v[lgsig +1 -i] = v[lgsig+1-i] * fen[i];
		v[i] = v[i] * fen[i];
		}
    if ((lgsig+1) > (2*larg1)) return(1);
    for ( i =lgsig+1; i <= 2*larg1; i=i+1)
		v[i] = 0;
    return(0);
	}

/********************************************************/
/*		PROCEDURE INIFILT1    		        */
/********************************************************/
/*   sous programme d'initialisation des filtres */
/*   triangulaires de frequences                  */

void INIFILT1(int lgsig)
	{
    int lgsig1,i,n;
    float f;
	
    lgsig1=lgsig/2;
    for  ( i=1; i <= lgsig1; i=i+1 )
		{
		nm[i] = 0;
		x1[i] = 0.;
		f = F_KHZ *500.* (float)(i) / (float) (lgsig1); // pq x 500 ?
		for ( n=1; n <= (NB_FILTRES+1); n = n+1)
			{
			if(!((f < (float)(fcoup[n])) || (f >= (float)(fcoup[n+1]))))
				{
				nm[i]= (float) n;
				x1[i] = (f-fcoup[n])/(fcoup[n+1]-fcoup[n]);
				x2[i] = 1-x1[i];
				}
			}
		}
	}



/********************************************************/
/*		PROCEDURE CINIT    		        */
/********************************************************/
void CINIT(int lgsig)
	{
    int i;
	
    ncoef=6;
    ncoef8=8;
	
	
    i = HAMJEAN(fen,lgsig);
    INIFILT1(lgsig);
	
	}

/********************************************************/
/*		PROCEDURE CFILT    		        */
/********************************************************/
/*  sous programme de calcul de l'energie de l'echantillon     */
/*  dans les filtres                                           */

void CFILT (int lgsig,float sig[],float en[],float *ener)
	{
    int i,i1,i2,n;
    float x11,x22;
	
    FFT(lgsig,sig);
    *ener=0.;
    for (  i=1; i <= (lgsig/2); i=i+1 )
		{
		i1=2*i+1;
		i2=i1+1;
		sig[i] = sig[i1]*sig[i1] + sig[i2]*sig[i2];
		*ener = *ener + sig[i];
		}
	
	
	
    for ( i=1; i <= NB_FILTRES; i=i+1)
		en[i] = 0.;
	
    for ( i=1; i <= (lgsig/2-1); i=i+1 )
		{
		i2 = (int) nm[i];
		if ( i2 != 0 ) 
			{
			i1 = i2 -1;
				// x1 et x2 sont des variables définies dans le .h
			x11 = x1[i];
			x22 = x2[i];
			if ( i1 != 0 )
				en[i1] = en[i1] + x22 * sig[i];
			if ( i2 <= NB_FILTRES)
				en[i2] = en[i2] + x11 * sig[i];
			}
		}
	
	
    for ( n = (NB_FILTRES+1); n <= NB_FILTRES8; n=n+1 )
		en[n] = 0.;
	}


/********************************************************/
/*		PROCEDURE CEPST    		        */
/********************************************************/
/*   sous programmes de calcul des coefficients cepstraux     */
/*   a partir des energies dans les filtres                   */

void CEPST(float en[],float ci[])
	{
    int i,j;
    for ( i=1; i <= ncoef8; i=i+1)
		{
		ci[i] = 0.;
		for ( j =1; j <= NB_FILTRES8; j=j+1 )
			ci[i] = ci[i] + en[j]*cos( (double) ( i*(j -.5)*pi/NB_FILTRES8 )   );
		}
	}

/********************************************************/
/*		PROCEDURE PARAMLEV    		        */
/********************************************************/
void paramlev(int lgsig,float sig[], float ucoeff0[], float * Energie)
	{
    int i,j;
    float ener,en[34];
	
    /*   initialisation des parametres de l'analyse */
    CINIT(lgsig);
    /*    for ( i=1; i <= lgsig; i=i+1 )
	sig[i] = sig[i+1]-sig[i];
	sig[lgsig+1]=0.;
	sig[lgsig+2]=0.;
	*/
	
    /*   calcul du fenetrage                        */
    i = FENSIG(sig,lgsig);
	
    /*   calcul des energies dans les filtres       */
    CFILT(lgsig, sig, en, &ener);
    
    if(Energie == NULL)
		{ /* CALCUL DES COEFS CEPSTRAUX */
		ener = log10 ((double) ener); /* ici on prend le log */
		for (j=0; j<34; j++) 
			{
			if (en[j] <= 0) { en[j] = 1.; } 
			en[j] = log10 ((double) en[j]); /* la aussi */ 
			}
		}
    else
		{
		Energie[0]=0;
		for(j=1; j<=NB_FILTRES;j++)
			{
			/*    if (en[j]<=0.0)
			en[j]=1.0;*/
			Energie[j]= en[j];
			if (FLAG_ENERGIE[j-1]!=0) 
				Energie[0]+=en[j];
			}
		}
    /*    calcul des coefficients cepstraux         */
    if (ucoeff0!=NULL)
		{
		CEPST(en,ucoeff0);
		ucoeff0[9] = ener;
		}
	}
