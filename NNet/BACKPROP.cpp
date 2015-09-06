#include "BACKPROP.hpp"
#include <stdlib.h>
#include <math.h>

//Misc flags
int ArchOn = AVERAGE;	//Set to 1=ALL, NONE, AVERAGE or WORST to control data sent to ARCHIVE file for graphical analysis

FILE *ARCHIVE;			//Archive training sequence

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
			fscanf(PFILE, "%x", &x);
			mask = 0x80;
			for (j = 0; j < 8; ++j) {
				if ((mask & x) > 0) {
					InPattern[pat][PGindx] = 1.0;
				}
				else {
					InPattern[pat][PGindx] = 0.0;
				}
				mask = mask / 2;
				++PGindx;
			}
		}
		for (i = 0; i < LayerSize[OutLayerIndx]; ++i) {
			fscanf(PFILE, "%lf", &inVal);
			Desired[pat][i] = inVal;
		}
	}
	fclose(PFILE);
	for (i = 0; i < NumPatterns; ++i) {
		PatPresOrder[i] = i;
	}
}

void BackProp::GetParms(char *PrmFileName) {
	FILE *PRMFILE;
	PRMFILE = fopen(PrmFileName, "r");
	if (PRMFILE == NULL) {
		printf("\nUnable to open Parameter file:%s\n", PrmFileName);
		printf("\nAttempting to open default file:PARMS1");
		PRMFILE = fopen("PARMS1", "r");
		if (PRMFILE == NULL) {
			printf("\nUnable to open Parameter file\n");
			exit(0);
		}
	}
	printf("\nLoading Parameters from file:%s\n", PrmFileName);
	fscanf(PRMFILE, "%lf", &Temperature);
	fscanf(PRMFILE, "%lf", &ETA);
	fscanf(PRMFILE, "%lf", &ALPHA);
	fscanf(PRMFILE, "%ld", &MAXITER);
	fscanf(PRMFILE, "%lf", &ERRTOL);
	fscanf(PRMFILE, "%ld", &NumLayers);
	for (int i = 0; i < NumLayers; ++i) {
		fscanf(PRMFILE, "%d", &LayerSize[i]);
		printf("Number of neurons in layer %d = %d\n", i, LayerSize[i]);
	}
	OutLayerIndx = NumLayers - 1;
	fclose(PRMFILE);
}

void BackProp::GetWeights() {
	FILE *WTFILE;
	char WtFileName[30];
	int i, j, k;
	double zWT;

	WTFILE = fopen("DEFAULT.WGT", "r");
	if (WTFILE == NULL) {
		printf("Unable to open weight file");
		exit(0);
	}

	for (i = 0; i < NumLayers - 1; ++i) {
		for (k = 0; k < LayerSize[i + 1]; ++k) {
			for (j = 0; j <= LayerSize[i]; ++j) {
				fscanf(WTFILE, "%lf", &zWT);
				W[i][j][k] = zWT;
			}
		}
	}

	fclose(WTFILE);
}

void BackProp::SetRandomWeights() {
	int i, j, k;
	double zWT;
	srand(6);
	for (i = 1; i < NumLayers; ++i) {
		for (k = 0; k < LayerSize[i]; ++k) {
			for (j = 0; j <= LayerSize[i - 1]; ++j) {
				zWT = rand();
				zWT = zWT / 2.0;
				W[i][j][k] = zWT / 4294967296.0;
				Wprev[i][j][k] = 0;
			}
		}
	}
}

void BackProp::GetInputs() {
	int i, j, k, a, b, c;
	unsigned int PatID;
	++CurrIter;
	++CurrPat;

	if (CurrPat >= NumPatterns) {
		CurrPat = 0;
		++Epoch;
	}

	if ((CurrPat == 0) && (SHUFFLE > 0)) {
		for (j = 0; j <= 5; ++j) {
			a = NumPatterns * rand() / 2147483648;
			b = NumPatterns * rand() / 2147483648;
			c = PatPresOrder[a];
			PatPresOrder[a] = PatPresOrder[b];
			PatPresOrder[b] = c;
		}

		if (100 * (Epoch / 100) == Epoch) {
			printf("Epoch: %d", Epoch);
			printf("Worst=%lf Avg=%lf\n", LastWorstErr, LastAvgErr);
		}
	}

	PatID = PatPresOrder[CurrPat];
	for (i = 0; i < LayerSize[0]; ++i) {
		Neuron[0][i] = InPattern[PatID][i];
	}
}

void BackProp::RunNet() {
	int lyr;
	int dNeuron;
	int sNeuron;
	double SumNet;
	double out;

	for (lyr = 1; lyr < NumLayers; ++lyr) {
		Neuron[lyr - 1][LayerSize[lyr - 1]] = 1.0;
		for (dNeuron = 0; dNeuron < LayerSize[lyr]; ++dNeuron) {
			SumNet = 0.0;
			for (sNeuron = 0; sNeuron <= LayerSize[lyr - 1]; ++sNeuron) {
				SumNet += Neuron[lyr - 1][sNeuron] * W[lyr][sNeuron][dNeuron];
			}
			out = Sigmoid(SumNet, Temperature);
			Neuron[lyr][dNeuron] = out;
		}
	}
}

double BackProp::HCalcDelta(int lyr, int j, double dNETj) {
	int k;
	double Delta, SUMk;
	SUMk = 0.0;
	for (k = 0; k < LayerSize[lyr + 1]; ++k) {
		SUMk += DELTAj[lyr + 1][k] * W[lyr + 1][j][k];
	}
	Delta = dNETj * SUMk;
	return Delta;
}

int BackProp::CalcErrors() {
	double dNETj;
	double MOMENTUM;
	int i, j;
	int LocalConvFlg = TRUE;
	unsigned int PatID;
	PatID = PatPresOrder[CurrPat];
	for (j = 0; j < LayerSize[OutLayerIndx]; ++j) {
		ERROR[j] = (Desired[PatID][j] - Neuron[OutLayerIndx][j]);
		if (fabs(ERROR[j]) >= ERRTOL) LocalConvFlg = FALSE;

		if (CurrPat == (NumPatterns - 1)) {
			AvgErr = AvgErr / NumPatterns;
			if (ArchOn == AVERAGE) {
				if (ARCGRAN * (Epoch / ARCGRAN) == Epoch) {
					fprintf(ARCHIVE, "%ld %lf %lf\n", Epoch, AvgErr, fabs(WorstErr));
				}
			}
			if (ArchOn == WORST) {
				if ((ARCGRAN*(Epoch / ARCGRAN)) == Epoch) {
					fprintf(ARCHIVE, "%ld %lf\n", Epoch, fabs(WorstErr));
				}
			}
			LastAvgErr = AvgErr;
			LastWorstErr = WorstErr;
			AvgErr = 0;
			WorstErr = 0.0;
		}
		else {
		}
		if (ArchOn == ALL) fprintf(ARCHIVE, "%ld %lf\n", CurrIter, fabs(ERROR[j]));
	}
	if (LocalConvFlg) {
		++ConvCount;
		if (ConvCount == 2 * NumPatterns) {
			ConvergeFlg = TRUE;
			printf("Epoch:%d", Epoch);
			printf("Worst=%lf Avg=%lf\n", LastWorstErr, LastAvgErr);
		}
	}
	else {
		ConvCount = 0;
	}
	return ConvergeFlg;
}

void BackProp::AdaptWeights() {
	double dNETj;
	double MOMENTUM;
	int lyr, i, j;
	unsigned int PatID;
	PatID = PatPresOrder[CurrPat];
	for (lyr = OutLayerIndx; lyr > 0; --lyr) {
		for (j = 0; j < LayerSize[lyr]; ++j) {
			dNETj = Neuron[lyr][j] * (1 - Neuron[lyr][j]);
			if (lyr == OutLayerIndx) {
				ERROR[j] = (Desired[PatID][j] - Neuron[lyr][j]);
				DELTAj[lyr][j] = ERROR[j] * dNETj;
			}
			else {
				DELTAj[lyr][j] = HCalcDelta(lyr, j, dNETj);
			}
		}
	}
	for (lyr = OutLayerIndx; lyr > 0; --lyr) {
		for (j = 0; j < LayerSize[lyr]; ++j) {
			for (i = 0; i <= LayerSize[lyr - 1]; ++i) {
				DELTA_Wij[lyr][i][j] = ETA * DELTAj[lyr][j] * Neuron[lyr - 1][i];
				MOMENTUM = ALPHA * (W[lyr][i][j] - Wprev[lyr][i][j]);
				Wprev[lyr][i][j] = W[lyr][i][j];
				W[lyr][i][j] = W[lyr][i][j] + DELTA_Wij[lyr][i][j] + MOMENTUM;
			}
		}
	}
}

void BackProp::SaveWeights(char *WgtName) {
	int lyr, s, d;
	double zWT;
	FILE *WEIGHTFILE;
	WEIGHTFILE = fopen(WgtName, "w");
	if (WEIGHTFILE == NULL) {
		printf("Unable to open weight file for output:%s\n", WgtName);
		exit(0);
	}
	printf("SAVING CALCULATED WEIGHTS:\n\n");
	fprintf(WEIGHTFILE, "0.00\n");
	fprintf(WEIGHTFILE, "%lf\n", Temperature);
	fprintf(WEIGHTFILE, "%d\n", NumLayers);
	for (lyr = 0; lyr < NumLayers; ++lyr) {
		fprintf(WEIGHTFILE, "%d\n", LayerSize[lyr]);
	}
	for (lyr = 1; lyr < NumLayers; ++lyr) {
		for (d = 0; d < LayerSize[lyr]; ++d) {
			for (s = 0; s <= LayerSize[lyr - 1]; ++s) {
				zWT = W[lyr][s][d];
				fprintf(WEIGHTFILE, "%lf\n", zWT);
			}
		}
	}
	fclose(WEIGHTFILE);
}

inline double BackProp::Sigmoid(double Net, double Tempr) {
	return 1.0 / (1.0 + exp(-Net / Tempr));
}

int BackProp::train(char *TrnFname, char *ParmFname) {
	if (ArchOn) ARCHIVE = fopen("archive.lst", "w");
	GetParms(ParmFname);
	LoadTrainingSet(TrnFname);
	SetRandomWeights();
	int Converged = 0;
	while ((!Converged) && (CurrIter < MAXITER))
	{
		GetInputs();
		RunNet();
		Converged = CalcErrors();
		AdaptWeights();
	}
	if (ArchOn) fclose(ARCHIVE);
	return Converged;
}