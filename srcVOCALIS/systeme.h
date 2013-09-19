
int* detecter_silences(float *audioContent, int durationSample, int *outDivSegmentation, int nbSegmentDiv, double facteur, int pausemin, double intervalle,  int samplerate );


#include "param2.h"
#include "vocal.h"
#include "global.h"
#include "voyelles.h"
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


