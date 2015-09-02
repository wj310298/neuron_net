#include "BACKPROP.hpp"
#include <stdlib.h>

BackProp::BackProp() {
	LastAvgErr = 1.0;
	LastWorstErr = 1.0;
	WorstErr = 0.0;
	AvgErr = 0.0;
	CurrIter = 0;
	Epoch = 0;
	CurrPat = -1;
	ConvCount = 0;
	ConvergeFlg = FALSE;
}

void BackProp::LoadTrainingSet(char *Fname) {
	FILE *PFILE;
	int PGindx;
	int x, mask;
	int pat, i, j;
	double inVal;
	int NumPatBytes;

	PFILE = fopen(Fname, "r");
	if (PFILE == NULL) {
		printf("\nUnable to open file\n");
		exit(0);
	}
	fscanf(PFILE, "%d", &NumPatterns);
	NumPatBytes = LayerSize[0] / 8;
	for (pat = 0; pat < NumPatterns; ++pat) {
		PGindx = 0;
		for (i = 0; i < NumPatBytes; ++i) {

		}
	}
}