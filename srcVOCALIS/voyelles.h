/*************************************************************************/
/*************************************************************************/
/**									**/
/**									**/
/**	Date de Creation : 		06/02/96			**/
/**	Date de derniere modif :	06/02/96			**/
/**	Systeme : Unix							**/
/**	Langage : C							**/
/**	Auteur : FP							**/
/**---------------------------------------------------------------------**/
/**	Description : 	Definition des structures liees aux voyelles	**/
/**---------------------------------------------------------------------**/
/**	Journal des versions :						**/
/**									**/
/**		Date	|	Remarques				**/
/**	----------------|-----------------------------------------------**/
/**	06/02/96	|	v0  Version initiale			**/
/**			|						**/
/**---------------------------------------------------------------------**/
/*************************************************************************/
/*************************************************************************/

#ifndef _VOYELLE_
#define _VOYELLE_

/*#include "global.h"*/

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT

/*************************/
/**  STRUCTURE VOYELLE  **/
/*************************/

struct voyelle
{
    int			no_voyelle;		/* no d'ordre de la voyelle dans le mot */
    int			no_segment;		/* no du segment dans le mot */
    int			debut_seg;		/* echantillon de debut du segment */
    int			fin_seg;		/* echantillon de fin du segment   */
    int			debut_max;		/* No de la trame la plus stable   */
    double		max_ds;			/* valeur de la derivée spectrale pour cette trame */
    int		confirm;		/* egal à 1 pour une voyelle confirmee, 0 sinon    */
    char		label[LONGUEUR_LABEL]; 	/* label d'etiquetage de la voyelle */
    float		coefs[NB_COEFS_VOY+1];	/* vecteur des coefficients */
} ;

/********************************/
/**  STRUCTURE LISTE_VOYELLES  **/
/********************************/

struct liste_voyelles
{
    char			nom[LONGUEUR_NOM];	/* fichier d'ou a ete extraite la voyelle */
    char			format[LONGUEUR_FORMAT];/* format d'enregistrement du fichier */
    int			no_mot;			/* numero du mot dans le fichier */
    struct voyelle		* pt_voyelle;		/* adresse de la voyelle */
    struct liste_voyelles	* suivant;		/* suivant ds la liste */
    struct liste_voyelles	* dernier;		/* dernier ds la liste */
};


/********************************/
/**  STRUCTURE SEGLOLA         **/
/********************************/
struct seglola
{
  int		no_label;		/* No du segment OGI */
  int		debut;			/* Trame de début en frame de 3 ms */ 
  int		fin;			/* Trame de fin en frame de 3 ms */
  int		flag;			/* flag = 0 si aucune detection dans le segment */
  char		label[LONGUEUR_LABEL];	/* Etiquette manuelle */
};


typedef struct voyelle			VOYELLE		;
typedef struct liste_voyelles		LISTE_VOYELLES	;
typedef struct seglola			SEGLOLA;

/********************************/
/**  STRUCTURE CPT_PRONONC     **/
/********************************/

struct cpt_prononc
{
    char			mot[LONGUEUR_NOM];
    int				cpt;
    struct cpt_prononc		* mot_suivant;
    struct cpt_prononc		* mot_dernier;
};
typedef struct cpt_prononc		CPT_PRONONC	;


#endif

