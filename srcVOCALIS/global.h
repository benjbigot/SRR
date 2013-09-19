/*************************************************************************/
/*************************************************************************/
/**									**/
/**									**/
/**	Date de Creation : 		31/10/95			**/
/**	Date de derniere modif :	03/07/96			**/
/**	Systeme : Unix							**/
/**	Langage : C							**/
/**	Auteur : FP							**/
/**---------------------------------------------------------------------**/
/**	Description : 	liste des variables globales	 		**/
/**			des fonctions spectrales			**/
/**									**/
/**---------------------------------------------------------------------**/
/**	Journal des versions :						**/
/**									**/
/**		Date	|	Remarques				**/
/**	----------------|-----------------------------------------------**/
/**	31/10/95	|	v0					**/
/**			|						**/
/**			|						**/
/**---------------------------------------------------------------------**/
/**	Liste des fonctions :	-					**/
/**									**/
/*************************************************************************/
/*************************************************************************/

#ifndef _GLOBAL_
#define _GLOBAL_

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
/*------------------------------------------------------*/
/*		Variables generales			*/
/*------------------------------------------------------*/

#define	LONGUE_CHAINE	200
#define LONGUEUR_NOM	30
#define LONGUEUR_LABEL	6
#define LONGUEUR_FORMAT 5

#define FORMAT_DATA "SIMPLE"


/*------------------------------------------------------*/
/*		Frequence des fichiers			*/
/*------------------------------------------------------*/
//#define F_KHZ			 8
//#define F_HZ			 8000



/*------------------------------------------------------*/
/*		Extensions des fichiers			*/
/*------------------------------------------------------*/
#define	EXT_DELTA 	 ".ds"
#define	EXT_CEPST 	 ".cep"
#define	EXT_PARAM 	 ".par"
#define EXT_VOY 	 ".voy"
#define EXT_LAB		 ".seglola"
#define EXT_NRJ		 ".nrj"
#define EXT_PLO		 ".res1"
#define EXT_PLO_OUT	 ".plo"

/*------------------------------------------------------*/
/*		Analyse spectrale			*/
/*------------------------------------------------------*/


#define BLOCK_SIZE		 128
#define NBMAX_SEG		 150
#define NB_VOY_MAX		 100	
#define	NB_FILTRES 		 24
#define NB_FILTRES8		 24
#define NB_COEFS		 19
#define NB_COEFS_VOY		 24

//#define LONG_FENETRE		 256
//#define	LARGEUR_FENETRE 	 256
// LARGEUR_FENETRE = 32 ms
#define	FACTEUR_RECOUVREMENT 	 0.50	/* definit le % de recouvrement */
					/* des fenetres pour le calcul  */
					/* de l'energie			*/

#endif
