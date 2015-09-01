#ifndef _PNET_HPP
#define _PNET_HPP
#define NEURONHIGH 1.0
#define NEURONLOW  0.0
#define TRUE	   1
#define FALSE	   0

class WEIGHTIMAGE {
	double data;
	int    sneuron;
	int    dneuron;
	WEIGHTIMAGE *next;
};

struct NETRESULTS {
	int    index;
	double value;
	char   character;
};

class NETFILEDATA {
private:
	double temperature;
	double threshold;
	int    Nlayers;
	int    neurons[MAXLAYERS];
	int    status;
	WEIGHTIMAGE *weights[MAXLAYERS];
	void   ADDweights(int l, int d, int s, double w);
	double GETweights(int l, int d, int s);

public:
	NETFILEDATA(void);
	int SetupNet(char *);
	inline double GetTemp(void) { return temperature; }
	inline double GetThreash(void) { return threshold; }
	inline int GetNlayers(void) { return Nlayers; }
	inline int GetLayerSize(int layer) { return neurons[layer]; }
	double GetWeight(int l, int d, int s) { return GETweights(l - 1, d, s); }
	int GetStatus(void) { return status; }
};

//*********************************************************************************************************************************************************************************************
// THE NEURON CLASS                                                                                                                                                                           *
//*********************************************************************************************************************************************************************************************
class NEURON {
private:
	static double temperature;		//Holds a single copy for all neurons
	static double threshold;		//Holds a single copy for all the neurons
	int    id;						//Holds a neuron identification number
	double out;						//Holds a neuron output value
	WEIGHT *weight1;				//Pointer to list of weights (head)
	WEIGHT *weightL;				//Pointer to list of weights (tail)
	int    BiasFlg;					//1 = Bias Neuron, 0 otherwise
	NEURON *next;					//Hook to allow neurons to be list members

public:

};
#endif