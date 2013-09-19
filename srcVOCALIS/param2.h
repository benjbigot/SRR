
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
/* Prototypes des fonctions de cepst2.c */
int HAMJEAN (float fen[],int lgsig);
int FENSIG ( float v[],int lgsig);
void INIFILT1(int lgsig);
void CINIT(int lgsig);
void CFILT (int lgsig,float sig[],float en[],float *ener);
void CEPST(float en[],float ci[]);
void paramlev(int lgsig,float sig[], float ucoeff0[],
	      float * Energie);

/* Prototypes des fonctions de fft2.c */
void R2TR(int INT,float B0[], float B1[]);
void R4TR(int INT,float B0[], float B1[], float B2[], float B3[]);
void R8TR(int INT,int NN, float BR0[], float BR1[], float BR2[], float BR3[],
	  float BR4[], float BR5[], float BR6[], float BR7[], float BI0[],
	  float BI1[], float BI2[], float BI3[], float BI4[], float BI5[],
	  float BI6[], float BI7[]);
void ORD1(int M,float B[]);
void ORD2(int M,float B[]);
void FFT (int NFFT,float B[]);

/* prototype des fonctions de cepstre2.c */
void ener_bruit (int p, int tsig[], float tbruit[], float *ebruit);

