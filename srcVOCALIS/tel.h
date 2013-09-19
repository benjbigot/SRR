#ifndef _TEL_
#define _TEL_

#define MAX_MOTS 64
#define LONG_MOT 60
#define LONG_FIC 100

/* Constantes */

#define SIZ_COEF 16384
#define VRAI 1
#define FAUX 0

/* variables globales */

/* static PRL_FIC *fprl ; */
static unsigned char coef[SIZ_COEF] ;


typedef struct
{
	char mot[LONG_MOT] ;
	long position ;
	int taille    ;
} INDEX ;


typedef struct  
{
	int code[9] ;
	char car[9] ;
} tab_car ;

#endif
