/*             *****************************
**********

  RENTRER DANS B LE SIGNAL DE LONGUEUR NFFT; REND DANS B:
  RE(B^(1)),IM(B^(1)),RE(B^(2))....,IM(B^(NFFT/2+1)).
  B DOIT ETRE DECLARE DE LONGUEUR NFFT+2 AU MINIMUM.
  
	DECLARATION B(NFFT+2)
*/
//#include "../Include/fft.h"
#include "fft.h"
#include <math.h>

void R2TR(int INT,float B0[], float B1[])
	{
    int K;
    float T;
    for ( K=1; K <= INT; K=K+1)
		{
		T = B0[K] + B1[K];
		B1[K] = B0[K] - B1[K];
		B0[K] = T;
		}
	}

void R4TR(int INT,float B0[], float B1[], float B2[], float B3[])
	{
    int K;
    float R0,R1;
    for ( K=1 ; K <= INT; K=K+1 )
		{
		R0 = B0[K] + B2[K];
		R1 = B1[K] + B3[K];
		B2[K] = B0[K] - B2[K];
		B3[K] = B1[K] - B3[K];
		B0[K] = R0 + R1;
		B1[K] = R0 - R1;
		}
	}




void R8TR(int INT,int NN, float BR0[], float BR1[], float BR2[], float BR3[],
		  float BR4[], float BR5[], float BR6[], float BR7[], float BI0[],
		  float BI1[], float BI2[], float BI3[], float BI4[], float BI5[],
		  float BI6[], float BI7[])
	{
    union U{
		int L[16];
		struct {
			int L16;
			int L15;
			int L14;
			int L13;
			int L12;
			int L11;
			int L10;
			int L9;
			int L8;
			int L7;
			int L6;
			int L5;
			int L4;
			int L3;
			int L2;
			int L1;
			} sl ;
		} SL;
		/*      EQUIVALENCE (L15,L(1)),(L14,L(2)),(L13,L(3)),(L12,L(4))
		EQUIVALENCE (L11,L(5)),(L10,L(6)),(L9,L(7)),(L8,L(8))
		EQUIVALENCE (L7,L(9)),(L6,L(10)),(L5,L(11)),(L4,L(12))
		EQUIVALENCE (L3,L(13)),(L2,L(14)),(L1,L(15))
	*/
    int K,JI,JL,JR,J1,J2,J3,J4,J5,J6,J7,J8,J9,J10;
    int INT8,J11,J12,J13,J14,JTHET,TH2;
    int K0,KL,J0,JLAST,J;
    float PIOVN,PR,PI,TR0,TI0,TR2,TI2,TR1,TI1,TR3,TI3;
    float TR4,TI4,TR5,TI5,TR6,TI6,TR7,TI7;
    float ARG,C1,C2,C3,C4,C5,C6,C7,S1,S2,S3,S4,S5,S6,S7;
    float T0,T1,T2,T3,T4,T5,T6,T7;
	
    SL.L[1] = NN/8;
    for ( K=2; K <= 15; K=K+1 )
		{
		if ((SL.L[K-1]-2) < 0)
			SL.L[K-1]=2;
		if ((SL.L[K-1]-2) <= 0)
			{
			SL.L[K]=2;
			}
		else
			SL.L[K] = SL.L[K-1]/2;
		}
	
    PIOVN=PII/ (float)(NN);
    JI=3;
    JL=2;
    JR=2;
	
    for ( J1=2; J1 <= SL.sl.L1; J1=J1+2) {
		for ( J2=J1; J2 <= SL.sl.L2; J2=J2+SL.sl.L1) {
			for ( J3=J2; J3 <= SL.sl.L3; J3=J3+SL.sl.L2) {
				for ( J4=J3; J4 <= SL.sl.L4; J4=J4+SL.sl.L3) {
					for ( J5=J4; J5 <= SL.sl.L5; J5=J5+SL.sl.L4) {
						for ( J6=J5; J6 <= SL.sl.L6; J6=J6+SL.sl.L5) {
							for ( J7=J6; J7 <= SL.sl.L7; J7=J7+SL.sl.L6) {
								for ( J8=J7; J8 <= SL.sl.L8; J8=J8+SL.sl.L7) {
									for ( J9=J8; J9 <= SL.sl.L9; J9=J9+SL.sl.L8) {
										for ( J10=J9; J10 <= SL.sl.L10; J10=J10+SL.sl.L9) {
											for ( J11=J10; J11 <= SL.sl.L11; J11=J11+SL.sl.L10) {
												for ( J12=J11; J12 <= SL.sl.L12; J12=J12+SL.sl.L11) {
													for ( J13=J12; J13 <= SL.sl.L13; J13=J13+SL.sl.L12) {
														for ( J14=J13; J14 <= SL.sl.L14; J14=J14+SL.sl.L13)  {
															for ( JTHET=J14; JTHET <= SL.sl.L15; JTHET=JTHET+SL.sl.L14)
																{
																TH2=JTHET-2;
																if (TH2 <= 0) 
																	{
																	for ( K=1; K <= INT; K=K+1)
																		{
																		T0=BR0[K] + BR4[K];
																		T1=BR1[K] + BR5[K];
																		T2=BR2[K] + BR6[K];
																		T3=BR3[K] + BR7[K];
																		T4=BR0[K] - BR4[K];
																		T5=BR1[K] - BR5[K];
																		T6=BR2[K] - BR6[K];
																		T7=BR3[K] - BR7[K];
																		BR2[K]=T0-T2;
																		BR3[K]=T1-T3;
																		T0=T0+T2;
																		T1=T1+T3;
																		BR0[K]=T0+T1;
																		BR1[K]=T0-T1;
																		PR=P7*(T5-T7);
																		PI=P7*(T5+T7);
																		BR4[K]=T4+PR;
																		BR5[K]=PI-T6;
																		BR6[K]=T4-PR;
																		BR7[K]=T6+PI;
																		}
																	
																	if ((NN-8) > 0) 
																		{ 
																		K0=8*INT+1;
																		KL=K0+INT-1;
																		for ( K=K0; K <= KL; K=K+1)
																			{
																			PR=P7*(BI2[K]-BI6[K]);
																			PI=P7*(BI2[K]+BI6[K]);
																			TR0=BI0[K]+PR;
																			TI0=BI4[K]+PI;
																			TR2=BI0[K]-PR;
																			TI2=BI4[K]-PI;
																			PR=P7*(BI3[K]-BI7[K]);
																			PI=P7*(BI3[K]+BI7[K]);
																			TR1=BI1[K]+PR;
																			TI1=BI5[K]+PI;
																			TR3=BI1[K]-PR;
																			TI3=BI5[K]-PI;
																			PR=TR1*C22-TI1*S22;
																			PI=TI1*C22+TR1*S22;
																			BI0[K]=TR0+PR;
																			BI6[K]=TR0-PR;
																			BI7[K]=TI0+PI;
																			BI1[K]=PI-TI0;
																			PR=-TR3*S22-TI3*C22;
																			PI=TR3*C22-TI3*S22;
																			BI2[K]=TR2+PR;
																			BI4[K]=TR2-PR;
																			BI5[K]=TI2+PI;
																			BI3[K]=PI-TI2;
																			}
																		}
																	}
																else
																	{
																	ARG=TH2*PIOVN;
																	C1= (float) cos((double)(ARG));
																	S1=(float) sin ((double)(ARG));
																	C2=C1*C1-S1*S1;
																	S2=C1*S1+C1*S1;
																	C3=C2*C1-S1*S2;
																	S3=C2*S1+C1*S2;
																	C4=C2*C2-S2*S2;
																	S4=C2*S2+C2*S2;
																	C5=C2*C3-S2*S3;
																	S5=S2*C3+S3*C2;
																	C6=C3*C3-S3*S3;
																	S6=S3*C3+S3*C3;
																	C7=C4*C3-S3*S4;
																	S7=S3*C4+S4*C3;
																	
																	INT8=8*INT;
																	J0=INT8*JR+1;
																	K0=JI*INT8+1;
																	JLAST=J0+INT-1;
																	for ( J=J0; J <= JLAST; J=J+1)
																		{
																		K=K0+J-J0;
																		TR1=BR1[J]*C1-BI1[K]*S1;
																		TI1=BR1[J]*S1+BI1[K]*C1;
																		TR2=BR2[J]*C2-BI2[K]*S2;
																		TI2=BR2[J]*S2+BI2[K]*C2;
																		TR3=BR3[J]*C3-BI3[K]*S3;
																		TI3=BR3[J]*S3+BI3[K]*C3;
																		TR4=BR4[J]*C4-BI4[K]*S4;
																		TI4=BR4[J]*S4+BI4[K]*C4;
																		TR5=BR5[J]*C5-BI5[K]*S5;
																		TI5=BR5[J]*S5+BI5[K]*C5;
																		TR6=BR6[J]*C6-BI6[K]*S6;
																		TI6=BR6[J]*S6+BI6[K]*C6;
																		TR7=BR7[J]*C7-BI7[K]*S7;
																		TI7=BR7[J]*S7+BI7[K]*C7;
																		
																		T0=BR0[J]+TR4;
																		T1=BI0[K]+TI4;
																		TR4=BR0[J]-TR4;
																		TI4=BI0[K]-TI4;
																		T2=TR1+TR5;
																		T3=TI1+TI5;
																		TR5=TR1-TR5;
																		TI5=TI1-TI5;
																		T4=TR2+TR6;
																		T5=TI2+TI6;
																		TR6=TR2-TR6;
																		TI6=TI2-TI6;
																		T6=TR3+TR7;
																		T7=TI3+TI7;
																		TR7=TR3-TR7;
																		TI7=TI3-TI7;
																		
																		TR0=T0+T4;
																		TI0=T1+T5;
																		TR2=T0-T4;
																		TI2=T1-T5;
																		TR1=T2+T6;
																		TI1=T3+T7;
																		TR3=T2-T6;
																		TI3=T3-T7;
																		T0=TR4-TI6;
																		T1=TI4+TR6;
																		T4=TR4+TI6;
																		T5=TI4-TR6;
																		T2=TR5-TI7;
																		T3=TI5+TR7;
																		T6=TR5+TI7;
																		T7=TI5-TR7;
																		BR0[J]=TR0+TR1;
																		BI7[K]=TI0+TI1;
																		BI6[K]=TR0-TR1;
																		BR1[J]=TI1-TI0;
																		BR2[J]=TR2-TI3;
																		BI5[K]=TI2+TR3;
																		BI4[K]=TR2+TI3;
																		BR3[J]=TR3-TI2;
																		PR=P7*(T2-T3);
																		PI=P7*(T2+T3);
																		BR4[J]=T0+PR;
																		BI3[K]=T1+PI;
																		BI2[K]=T0-PR;
																		BR5[J]=PI-T1;
																		PR=-P7*(T6+T7);
																		PI=P7*(T6-T7);
																		BR6[J]=T4+PR;
																		BI1[K]=T5+PI;
																		BI0[K]=T4-PR;
																		BR7[J]=PI-T5;
																		}
																	JR=JR+2;
																	JI=JI-2;
																	if((JI-JL) <= 0)
																		{ 
																		JI=2*JR-1;
																		JL=JR;
																		}
																	}
								}}}}}}}}}}}}}}}
}

void ORD1(int M,float B[])
	{
    int K,KL,N,J;
    float T;
    K=4;
    KL=2;
    N = (int) pow( (double) (2), (double) (M));
    for ( J=4; J <= N; J=J+2 )
		{
		if ((K-J) > 0) 
			{ 
			T=B[J];
			B[J]=B[K];
			B[K]=T;
			}
        K=K-2;
        if ((K-KL)  <= 0)
			{
			K=2*J;
			KL=J;
			}
		}
	}

void ORD2(int M,float B[])
	{
    union U{
		int L[16];
		struct {
			int L16;
			int L15;
			int L14;
			int L13;
			int L12;
			int L11;
			int L10;
			int L9;
			int L8;
			int L7;
			int L6;
			int L5;
			int L4;
			int L3;
			int L2;
			int L1;
			} sl ;
		} SL;
    int N,K,IJ,J1;
    int J2,J3,J4,J5,J6,J7,J8,J9,J10,J11,J12,J13,J14,JI;
    float T;
    N = (int) pow( (double) (2), (double) (M));
	
    SL.L[1]=N;
    for ( K=2; K <= M; K=K+1)
		SL.L[K]=SL.L[K-1]/2;
    for ( K=M; K <= 14; K=K+1)
		SL.L[K+1]=2;
    IJ=2;
	
    for ( J1=2; J1 <= SL.sl.L1; J1=J1+2) {
		for ( J2=J1; J2 <= SL.sl.L2; J2=J2+SL.sl.L1 ) {
			for ( J3=J2; J3 <= SL.sl.L3; J3=J3+SL.sl.L2 ) {
				for ( J4=J3; J4 <= SL.sl.L4; J4=J4+SL.sl.L3 ) {
					for ( J5=J4; J5 <= SL.sl.L5; J5=J5+SL.sl.L4 ) {
						for ( J6=J5; J6 <= SL.sl.L6; J6=J6+SL.sl.L5 ) {
							for ( J7=J6; J7 <= SL.sl.L7; J7=J7+SL.sl.L6 ) {
								for ( J8=J7; J8 <= SL.sl.L8; J8=J8+SL.sl.L7 ) {
									for ( J9=J8; J9 <= SL.sl.L9; J9=J9+SL.sl.L8 ) {
										for ( J10=J9; J10 <= SL.sl.L10; J10=J10+SL.sl.L9 ) {
											for ( J11=J10; J11 <= SL.sl.L11; J11=J11+SL.sl.L10 ) {
												for ( J12=J11; J12 <= SL.sl.L12; J12=J12+SL.sl.L11 ) {
													for ( J13=J12; J13 <= SL.sl.L13; J13=J13+SL.sl.L12 ) {
														for ( J14=J13; J14 <= SL.sl.L14; J14=J14+SL.sl.L13 ) {
															for ( JI=J14; JI <= SL.sl.L15; JI=JI+SL.sl.L14 ) {
																if ((IJ-JI) < 0)
																	{ 
																	T=B[IJ-1];
																	
																	B[IJ-1]=B[JI-1];
																	
																	B[JI-1]=T;
																	
																	T=B[IJ];
																	B[IJ]=B[JI];
																	B[JI]=T;
																	
																	}
																IJ=IJ+2;
		}}}}}}}}}}}}}}}
	
	}


void FFT (int NFFT,float B[])
	{
    int M,MM,N,NN,N8POW,INT,I,IT;
    float PI8,T;
    double XX;
	
    PII= (float) (4.* atan( (double) (1.)));
    PI8=PII/8.;
    P7=(float) (1./sqrt ( (double)(2.)));
    P7TWO=2.*P7;
    C22= (float) cos ((double)(PI8));
    S22=(float) sin ((double)(PI8));
    PI2=2.*PII;
    XX= (double) NFFT;
    M=(int) ( log(XX)/ log((double)(2.))+.5);
    MM = (int) pow( (double) (2), (double) (M));
 //   if (MM != NFFT)
		//printf("Erreur en FFT mettre une puissance de 2\n");
	N=NFFT;
    N8POW=M/3;
    if ((M-N8POW*3-1) > 0) 
		{
		NN=4;
		INT=N/NN;
		
		R4TR(INT,&(B[0]),&(B[INT]),&(B[2*INT]),&(B[3*INT]));
		
		}
    else
		{
		if ((M-N8POW*3-1) == 0)
			{
			NN=2;
			INT=N/NN;
			
			R2TR(INT,&(B[0]),&(B[INT]));
			
			
			}
		else
			NN=1;
		
		}
	
    if (N8POW > 0) 
		{
		for ( IT=1; IT <= N8POW; IT=IT+1)
			{
			NN=NN*8;
			INT=N/NN;
			R8TR(INT,NN,&(B[0]),&(B[INT]),&(B[2*INT]),&(B[3*INT]),
				&(B[4*INT]),&(B[5*INT]),&(B[6*INT]),&(B[7*INT]),&(B[0]),
				&(B[INT]),&(B[2*INT]),&(B[3*INT]),&(B[4*INT]),&(B[5*INT]),
				&(B[6*INT]),&(B[7*INT])); 
			
			}
		}
	
    ORD1(M,B);
	
    ORD2(M,B);
	
	
    T=B[2];
    B[2]=0.;
    B[NFFT+1]=T;
    B[NFFT+2]=0.;
    for ( I=4; I <= NFFT; I=I+2)
		B[I]= -B[I];
	}


