/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**                                                                     **/
/**     Date de Creation :              31/10/95                        **/
/**     Date de derniere modif :        06/12/02                        **/
/**     Systeme : Unix                                                  **/
/**     Langage : C                                                     **/
/**     Auteur : FP                                                     **/
/**---------------------------------------------------------------------**/
/**     Description :   procedures ouvrant les fichiers nrj,            **/
/**                     derivee_spectrale, segments                     **/
/**                     en LECTURE s'ils existent                       **/
/**                     et lance leur creation s'ils n'existent pas     **/
/**                     Procedure principale gerant la detection        **/
/**                                                                     **/
/**---------------------------------------------------------------------**/
/**     Journal des versions :                                          **/
/**                                                                     **/
/**             Date    |       Remarques                               **/
/**     ----------------|-----------------------------------------------**/
/**     31/10/95        |       v0                                      **/
/**     08/12/95        |       v1 debuggee ; la derivee spectrale      **/
/**                     |       est en unsigned int                     **/
/**     07/02/96        |       v1.1 structure voyelle creee            **/
/**     juin 2000       |       v2.0 Interfacage avec Snack et Tcl/Tk   **/
/**     déc 2002        |       v2.1 Prise en compte F0 ds detectionV   **/
/**                     |                                               **/
/**---------------------------------------------------------------------**/
/**     Liste des fonctions : OBSOLETE - A REVOIR                       **/
/**                             copie_segmentation                      **/
/**                             copie_ds                                **/
/**                             copie_cepstraux                         **/
/**                             initialiser_donnees                     **/
/**                             lancer_detection                        **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/

#include "systeme.h"

/**************************************************************************/
/***	DECLARATION DES VARIABLES GLOBALES			                    ***/
/**************************************************************************/
//FILE * fd_err;
//FILE * fd_err2;
//extern int F_HZ;
//extern int F_KHZ;
extern float F_HZ;
extern float F_KHZ;
int LARGEUR_FENETRE;
extern int verbose;	 

/*********************************************************/
/*		PROCEDURE copie_segmentation			*/
/*********************************************************/
extern void copie_segmentation( FILE * fd, int * tab_seg, int taille_seg ){
	
	int i , compt ;
	compt = 0 ;
	for( i = 0 ; i < taille_seg ; i ++ ){
		compt++ ;
		if( compt > 10 ){
			fprintf( fd , "\n" ) ;
			compt = 1 ;
		}
		fprintf( fd , "%d ", tab_seg[i] ) ;
	}
	fprintf( fd , "\n" ) ;	
	return ;	
}

/****************************************************/
/*				PROCEDURE copie_ds					*/
/****************************************************/
extern void copie_ds( FILE * fd, double * tab_seg, int taille_seg ){
	int i , compt ;
	compt = 0 ;
	for( i = 1 ; i <= taille_seg ; i ++ ){
		fprintf( fd , "%lf\n", tab_seg[i] ) ;
	}

	return ;
}


/************************************************/
/*				PROCEDURE detection				*/
/************************************************/
int * detection(float * tableau, int Snd_freq, int Snd_length, int Frate, int facteurFreq, int * outDivSegmentation, int * tab_sil, float * f0, int nbSegment){

	if (verbose){
		printf("snd : %i\n", Snd_freq );
		printf("snd : %i\n", Snd_length );
		printf("Frate : %i\n", Frate);
		printf("Facteur : %i\n", facteurFreq);
	}
	
	
	if(verbose){
		puts("==========================================================");		
		puts("---------------- Vocalic segmentation --------------------");
		puts("==========================================================");	
	}
	

	

		
	int retour;	
	int	*tab_seg;
	int i, taille, taille_sil, nb_enreg;	
	int taille_seg;
	taille_seg	= nbSegment;
	LISTE_VOYELLES * pt_liste_voyelles;
	LISTE_VOYELLES * pt_voyelle_courante;		
	int flag_delta;
	int flag_seg;
	int position;
	float *alpha;
	float *beta;
	float *energie;
	float energie_max;
	float *derivee_spectrale;
	float *timef0;
	int nb_fenetres,nb_fenetres_silence, nb_valeursf0max, nbF0;
	int nb_voyelles;
	int nb_mots;
	int recouvrement;
	int debut_index_delta;
	long int pos_debut_seg;
	long int pos_debut_delta;
	int g;
	int frame, nb_consonnes;
	int segment_voyelle[2];
	int * voyelContainer;
	
	/*------------------------------------------------------------*/
	/*				 Initialisations des donnees				  */
	/*------------------------------------------------------------*/	
	flag_delta = 0;
	flag_seg   = 0;
	pos_debut_seg = 0;
	pos_debut_delta = 0;
	debut_index_delta = 0;
	nbF0 = Snd_length / (Snd_freq/100) ; //nb de valeur dans le vecteur f0.
	/*------------------------------------------------------------*/
	/*				   Initialisation Signal					  */
	/*------------------------------------------------------------*/	
	LARGEUR_FENETRE = 32 * F_KHZ; // Fenetre de 32 ms	
	if (fabs(Frate - 1.0) > 1e-5) 
	/*------------------------------------------------------------*/
	/*	 Calcul de la largeur de la fenetre d'analyse spectrale   */
	/*			et du nombre de fenetres correspondant			  */
	/*------------------------------------------------------------*/
	recouvrement = (int) (LARGEUR_FENETRE * FACTEUR_RECOUVREMENT);	
	i = 0;
	/*------------------------------------------------------------*/
	/*		 Allocations des tableaux de donnees				  */
	/*------------------------------------------------------------*/
	nb_fenetres		= (int) (floor((double) (Snd_length/(LARGEUR_FENETRE-recouvrement)) ))-1;
	nb_valeursf0max = (int) ((Snd_length + F_HZ)/F_KHZ/10);		
	energie				= (float *) malloc (nb_fenetres*(NB_FILTRES+1)*sizeof(float));
	derivee_spectrale	= (float *) malloc ((nb_fenetres+1)*sizeof(float));
	alpha				= (float *) malloc ((nb_fenetres+1)*sizeof(float));
	beta				= (float *) malloc ((nb_fenetres+1)*sizeof(float));
	timef0 = (float *) malloc ((nb_valeursf0max+1) * sizeof(float));

	int nn;
	float timeSlot = 0.0;
	for (nn = 0; nn < nbF0; nn++) {
		timef0[nn] = timeSlot;
		timeSlot += 0.01;
	}
		
	if ((energie==NULL) || (derivee_spectrale==NULL)){
		puts("(detection) Erreur : Depassement memoire en Reallocation");
		return(0);
	}

	/*------------------------------------------------------------*/
	/*				   Calcul de l'ENERGIE						  */
	/*------------------------------------------------------------*/
	calculer_energie(tableau, recouvrement, energie, nb_fenetres, &energie_max);
	
	/*------------------------------------------------------------*/
	/*					 Calcul des SEGMENTS					  */
	/*------------------------------------------------------------*/
	
	tab_seg = (int *) malloc(nbSegment * sizeof(int));
	
	int it_seg = 0; // indice commence ˆ 0.
	for (it_seg = 0 ; it_seg < nbSegment; it_seg++){
		tab_seg[it_seg] = outDivSegmentation[it_seg];
	}
	//-----------------------------------------------------------------------
	// verification de la conservation des valeurs contenues dans le vecteur.
	// Utilisation de la fonction memcpy().
	//-----------------------------------------------------------------------
		
	nb_fenetres_silence = calculer_duree_silence(tab_seg, tab_sil, taille_seg);
	
	if (verbose){
		printf("nb silence %i\n", nb_fenetres_silence);
	}
	
	/*------------------------------------------------------------*/
	/*			 Calcul de la DERIVEE SPECTRALE 				  */
	/*------------------------------------------------------------*/

	calculer_critere(energie, derivee_spectrale, alpha, beta, nb_fenetres, nb_fenetres_silence);
	
	/*------------------------------------------------------------*/
	/*					   DETECTION VOYELLES					  */
	/*------------------------------------------------------------*/

	if (retour=initialiser_liste_voyelles2("SIMPLE", 0, &pt_liste_voyelles)){
		printf("pb intialisation retour %i\n", retour);
		// si probleme d'allocation de la structure alors retourne 1
		return (1);
	}
	else{
		if (verbose){ puts("initlisation ok");}
	}

	if((retour=detecter_voyelles(derivee_spectrale, &pt_liste_voyelles, nb_fenetres, facteurFreq))!=0){
		puts("pb detection voyelle phase 1");
		return(1);
	}

	if (pt_liste_voyelles->dernier != NULL)	{
		nb_voyelles = pt_liste_voyelles->dernier->pt_voyelle->no_voyelle;		
	}
	else{		
		nb_voyelles = 0;		
	}
	if (verbose){
		printf("nb de voyelle apres premiere phase : %i\n", nb_voyelles);
	}
	
	/*------------------------------------------------------------*/
	/*					Calcul des CEPSTRAUX des Voyelles		  */
	/*------------------------------------------------------------*/
	
	if (nb_voyelles > 0){
		pt_voyelle_courante = pt_liste_voyelles->suivant;
		while(pt_voyelle_courante != NULL){
			if((retour=extraire_segment_voyelle(pt_voyelle_courante->pt_voyelle,tab_seg, taille_seg))!=0){
				if (verbose) printf("(detection)\tErreur %d : Voyelle no %d trouvee au dela du signal\n",retour, pt_voyelle_courante->pt_voyelle->no_voyelle);				
			}
			else{									
				/*---------------------------------*/
				/*	Confirmation des voyelles	   */
				/*---------------------------------*/
				
				if (confirmer_une_voyelle(pt_voyelle_courante->pt_voyelle, derivee_spectrale, alpha, beta)){
					pt_voyelle_courante->pt_voyelle->confirm = 1;
				}
												
				position = (int) (pt_voyelle_courante->pt_voyelle->debut_max * recouvrement);
				frame = (LARGEUR_FENETRE-recouvrement);
				if (tab_sil[pt_voyelle_courante->pt_voyelle->no_segment] < 1){
					pt_voyelle_courante->pt_voyelle->confirm = 0;
				}
				else{
					tab_sil[pt_voyelle_courante->pt_voyelle->no_segment] = -5; // Code de label voyelle = -5
				}
				

				pt_voyelle_courante->pt_voyelle->debut_max = pt_voyelle_courante->pt_voyelle->debut_max * frame;	
				segment_voyelle[0] = (position>frame?position-frame:1);
				segment_voyelle[1] = (position<Snd_length-frame?position+frame:Snd_length);

				
				if (isvoiced(segment_voyelle, timef0, f0, nbF0)){ 
				}
				else{
					pt_voyelle_courante->pt_voyelle->confirm = 0;
				}				
			}
			pt_voyelle_courante = pt_voyelle_courante->suivant;			
		}


		/*------------------------------------------------------------*/
		/*			Recopie des donnees ds les fichiers 			  */
		/*------------------------------------------------------------*/
		voyelContainer = (int *) malloc(sizeof(int) * (nb_voyelles+1));	
		
		LISTE_VOYELLES * pt_voyelle_courante;
		pt_voyelle_courante = pt_liste_voyelles->suivant;
		int no_voyelle_precedente;
		int indice_voyelle = 1;
		while (pt_voyelle_courante != NULL)	{
			if (no_voyelle_precedente != pt_voyelle_courante->pt_voyelle->no_voyelle){
				if (pt_voyelle_courante->pt_voyelle->confirm == 1){
					position = pt_voyelle_courante->pt_voyelle->debut_max ;
					voyelContainer[indice_voyelle]  = position;
					indice_voyelle++;
				}
				no_voyelle_precedente = pt_voyelle_courante->pt_voyelle->no_voyelle;
			}
			pt_voyelle_courante = pt_voyelle_courante->suivant;
		}		
		voyelContainer[0] = indice_voyelle - 1;
		if (verbose){
			printf("nb voyelle final : %i\n", voyelContainer[0]);
		}
	}

	/*------------------------------------------------------------*/
	/*			Désallocation des tableaux de donnees			  */
	/*------------------------------------------------------------*/
	
	free( tableau );
	free( energie);
	free( alpha);
	free( beta);
	free( derivee_spectrale);
	free( timef0);
	/*------------------------------------------------------------*/
	/*			Désallocation de pt_liste_voyelles                */
	/*------------------------------------------------------------*/
	free_liste_voyelles(&pt_liste_voyelles);
	return voyelContainer;
}
