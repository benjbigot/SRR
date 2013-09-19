/*************************************************************************/
/*************************************************************************/
/**									                                    **/
/**									                                   **/
/**	Date de Creation : 		06/02/96                                   **/
/**	Date de derniere modif :	10/10/96                                   	**/
/**	Systeme : Unix						                                   	**/
/**	Langage : C					                                   	**/
/**	Auteur : FP					                                   	**/
/**---------------------------------------------------------------------**/
/**	Description : 	liste des macros	                                    		**/
/**								                                   **/
/**---------------------------------------------------------------------**/
/**	Journal des versions :				                                   	**/
/**								                                   	**/
/**		Date	|	Remarques		                                   		**/
/**	----------------|-----------------------------------------------**/
/**	06/02/96	|	v0				                                   	**/
/**			|					                                   	**/
/**			|					                                   	**/
/**---------------------------------------------------------------------**/
/**	Liste des fonctions :	-				                                   	**/
/**								                                   	**/
/*************************************************************************/
/*************************************************************************/

#ifndef _MACROS_
#define _MACROS

#define new_voyelle() (VOYELLE *) malloc (sizeof(VOYELLE))
#define new_chainage_voyelle() (LISTE_VOYELLES *) malloc (sizeof(LISTE_VOYELLES))
#define new_cpt_prononc() (CPT_PRONONC *) malloc(sizeof(CPT_PRONONC))

#define new_label() (SEGLOLA *) malloc (sizeof(SEGLOLA))
#define new_chainage_label() (LISTE_VOYELLES *) malloc (sizeof(LISTE_VOYELLES))

#endif
