#ifndef _BACKPROP_HPP
#define _BACKPROP_HPP

#include <stdio.h>

#define MAXLAYERS 4
#define MAXNEURONS 120
#define MAXPATTERNS 80
#define SIGMOID 0
#define STEPFN 1
#define ARCGRAN 1
#define NONE 0
#define ALL 1
#define AVERAGE 2
#define WORST 3
#define TRUE 1
#define FALSE 0
#define SHUFFLE 0

class BackProp {
private:
	double W[MAXLAYERS][MAXNEURONS][MAXNEURONS];		//WEIGHTS MATRIX
	double Wprev[MAXLAYERS][MAXNEURONS][MAXNEURONS];	//previous WEIGHTS for momentum
	double Neuron[MAXLAYERS][MAXNEURONS];
	double DELTAj[MAXLAYERS][MAXNEURONS];
	double DELTA_Wij[MAXLAYERS][MAXNEURONS][MAXNEURONS];
	double ERROR[MAXNEURONS];
	double WorstErr;
	double AvgErr;
	double LastAvgErr;
	double LastWorstErr;
	//Topology
	int NumLayers;				//Number of layers
	int OutLayerIndx;			//array index of last layer
	int LayerSize[MAXLAYERS];	//Number of neurons in each layer
	//Pattern data
	double InPattern[MAXPATTERNS][MAXNEURONS];	//Input values for each pattern
	double Desired[MAXPATTERNS][MAXNEURONS];	//Desired value for each pattern/output
	unsigned int PatPresOrder[MAXPATTERNS];		//Order to present patterns to net
	unsigned long int CurrIter;					//Current iterations number
	long int Epoch;
	int NumPatterns;							//Total patterns in training set
	int CurrPat;								//Current pattern used in training
	int ConvCount;								//The number of consecutive patterns within tolerance
	int ConvergeFlg;							//Flag indicates convergence has occurred
	//PARMS
	double Temperature;							//For sigmoid
	double ETA;									//Learning rate
	double ALPHA;								//Momentum
	double ERRTOL;								//Min error tolerance required for convergence
	unsigned long int MAXITER;					//Max iterations to do before stopping

public:
	BackProp(void);
	void LoadTrainingSet(char *Fname);
	void GetInputs(void);
	void RunNet(void);
	void GetParms(char *);
	void GetWeights(void);
	double Sigmoid(double Net, double Tempr);
	void SetRandomWeights(void);
	double HCalcDelta(int lyr, int j, double dNETj);
	int CalcErrors(void);
	void AdaptWeights(void);
	void SaveWeights(char *);
	int train(char *, char *);
	inline long int QueryEpoch() { return Epoch; }
	inline double QueryTemparature() { return Temperature; }
	inline double QueryEta() { return ETA; }
	inline double QueryAlpha() { return ALPHA; }
};

//Misc flags
int ArchOn = AVERAGE;	//Set to 1=ALL, NONE, AVERAGE or WORST to control data sent to ARCHIVE file for graphical analysis

FILE *ARCHIVE;			//Archive training sequence
#endif