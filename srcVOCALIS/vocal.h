/*#include "../Include/global.h"
#include "../Include/erreur.h"
#include "../Include/tel.h"
#include "../Include/voyelles.h"*/

#include "global.h"
//#include "erreur.h"
//#include "tel.h"
#include "voyelles.h"



#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

#ifdef TOTO
/* prototypes des fonctions du fichier travail.c */


extern void charge_segments( FILE *fd_seg ,  long pos_debut_seg , int taille_seg , int tableau_seg[] ) ;
extern void charge_segments2( FILE *fd_seg ,  long pos_debut_seg , int *taille_seg , int tableau_seg[] ) ;
extern void charge_reels( FILE *fd_seg , long  pos_debut_seg , int taille_seg ,float tableau_seg[] ) ;
extern void charge_double( FILE *fd_seg ,  long pos_debut_seg ,int  taille_seg , double tableau_seg[] ) ;
extern void lecture_chaine(  char *mot ,int  max  ,FILE *  fd );


/* prototypes des fonctions du fichier index.c */

extern void reservation_index( FILE * fd , int nb_mots );
extern void recopie_index( FILE * fd , INDEX * table , int nb_mots );
extern int recherche_index( INDEX * table , char * mot , long * pos_debut ,  int * taille ,
			   int * init , int nb_mots );
extern void creation_index( FILE * fd , INDEX * tab_index , int * nb_mots );

/* prototypes des fonctions du fichier derivee.c */

//extern void calculer_energie(int *Signal, int Recouvrement, float *Energie, int Nb_fenetres, float *Energie_max);
extern void calculer_energie(float *Signal, int Recouvrement, float *Energie, int Nb_fenetres, float *Energie_max);
extern void calculer_energie_seg(int *Signal, int debut_seg,  int fin_seg,  float *Energie);
extern double calculer_derivee( float * Energie);
extern void calculer_derivee_spectrale(float *Energie, double *Derivee_spectrale, int Nb_fenetres);
extern int extraire_segment_trame(int No_trame, int *Tab_seg, int Nb_seg, float *Energie_segment, 
				  int *Signal);

/* prototypes des fonctions du fichier detecter.c */

extern void copie_segmentation( FILE * fd,  int * tab_seg, int taille_seg );
extern void copie_ds( FILE * fd, double * tab_seg, int taille_seg );
extern void initialiser_donnees(char *File_name, FILE **Fd_delta, 
			 FILE **Fd_seg, int *Flag_delta, int *Flag_seg);
extern void copie_cepstraux( FILE * fd, float * tab_cepstraux, int taille_cepstraux );


/* prototypes des fonctions du fichier analyser.c */

extern char localiserPulses(double tabDeltaE[], int nbtabDeltaE, int t_marquePulse[300][2],int *nbt_pulse);
extern char localiser_maxima(double TabDeltaE[], int Indice_debut, int Indice_fin,
			     LISTE_VOYELLES **  Liste_voyelles);
extern char localiser_voyelles(double tabDeltaE[], int t_marquePulse[300][2],
			       int nbt_pulse,LISTE_VOYELLES ** Liste_voyelles);

extern int confirmer_une_voyelle(VOYELLE * Pt_voyelle, float * Aire_moyenne, float *alpha, float *beta);
/* extern int confirmer_une_voyelle(VOYELLE * Pt_voyelle, float * Energie, float *Energie_max);*/
//extern int confirmer_une_voyelle(VOYELLE * Pt_voyelle, double Aire_moyenne, float *Alpha, float *Beta);

//extern char detecter_voyelles(double * Derivee, LISTE_VOYELLES ** Liste_voyelles, int Nb_fenetres);
extern char detecter_voyelles(double * Derivee, LISTE_VOYELLES ** Liste_voyelles, int Nb_fenetres, int Seuil);
extern char extraire_segment_voyelle(VOYELLE *Pt_voyelle, int *Tab_seg,int Nb_seg);
//extern char extraire_sommets(double *Aire, LISTE_VOYELLES **Liste_voyelles,int  NbTrames);
extern char extraire_sommets(double *Aire, LISTE_VOYELLES **Liste_voyelles,int  NbTrames, int Seuil);



/* prototypes des fonctions du fichier outils.c */

extern void copie_une_voyelle( FILE * Fd, VOYELLE * Pt_voyelle);

//extern void copie_voyelles( FILE * Fd, LISTE_VOYELLES * Pt_liste_voyelles);
extern double * copie_voyelles( LISTE_VOYELLES * Pt_liste_voyelles, int nb_voyelles);


extern VOYELLE * creer_une_voyelle(int No_voyelle);
extern LISTE_VOYELLES * creer_chainon_voyelle(VOYELLE *Pt_voyelle);
extern int maj_liste_voyelles(LISTE_VOYELLES ** Liste_voyelles,VOYELLE * Pt_voyelle, int No_voyelle);
extern int initialiser_liste_voyelles(char * File_name, char * Format, int No_mot,
				      LISTE_VOYELLES ** Pt_liste_voyelles);
extern CPT_PRONONC * creer_cpt_prononc();
extern int maj_cpt_prononc(CPT_PRONONC ** Cpt_prononc, char *Mot, CPT_PRONONC ** Pt_cpt_mot_courant);
extern int ajouter_cpt_prononc(CPT_PRONONC * Cpt_prononc, char *Mot, CPT_PRONONC ** Pt_cpt_mot_courant );

#endif