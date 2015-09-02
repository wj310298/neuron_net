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
	NEURON(void) {
		id = 0;
		out = 0;
	}
	NEURON(int ident, int bias = 0) {
		id = ident;
		out = 0;
		BiasFlg = bias;
	}

	void Calc(void);
	inline void SetNext(NEURON *N) { next = N; }
	inline NEURON *GetNext(void) { return next; }
	void SetWeight(double Wght, NEURON *SrcPtr);
	inline int GetId(void) { return id; }
	inline double GetOut(void) { return out; }
	inline void SetTemperature(double tmpr) { temperature = tmpr; }
	inline void SetThreshold(double thrsh) { threshold = thrsh; }
	inline void SetOut(double val) { out = val; }
	inline int IsBias() { return BiasFlg; }
};
double NEURON::temperature = 0.0;
double NEURON::threshold = 0.0;

//*********************************************************************************************************************************************************************************************
// THE WEIGHT CLASS                                                                                                                                                                           *
//*********************************************************************************************************************************************************************************************
class WEIGHT {
private:
	NEURON *SRCneuron;
	double WtVal;
	WEIGHT *next;

public:
	WEIGHT(double W, NEURON *SN) {
		SRCneuron = SN;
		WtVal = W;
	}
	inline NEURON *GetSRCNeuron(void) { return SRCneuron; }
	inline double GetWeight(void) { return WtVal; }
	inline void SetNext(WEIGHT *W) { next = W; }
	inline WEIGHT *GetNext(void) { return next; }
};

//*********************************************************************************************************************************************************************************************
// THE LAYER CLASS                                                                                                                                                                            *
//*********************************************************************************************************************************************************************************************
class LAYER {
private:
	int          LayerID;
	unsigned int Ncount;
	NEURON       *Neuron1;
	NEURON       *NeuronL;
	LAYER        *next;

public:
	LAYER(int layer_id, NETFILEDATA *netdata);
	int SetWeight(NEURON *PrevNeuron, NETFILEDATA *netdats);
	inline void SetNext(LAYER *Nlayer) { next = Nlayer; }
	inline int GetLayerID(void) { return LayerID; }
	inline NEURON *GetFirstNeuron() { return Neuron1; }
	inline LAYER *GetNext(void) { return next; }
	inline unsigned int GetCount(void) { return Ncount; }
	void Calc(int);
};

//*********************************************************************************************************************************************************************************************
// THE NETWORK CLASS                                                                                                                                                                          *
//*********************************************************************************************************************************************************************************************
class NETWORK {
private:
	int         Alive;
	NETFILEDATA netdata;
	int         Nlayers;
	LAYER       *INlayer;
	LAYER       *OUTlayer;
	NETWORK     *next;

public:
	NETWORK(void) { Alive = 0; }
	int Setup(char *);
	void ApplyVector(unsigned char *, int);
	void RunNetWork(void);
	int RequestNthOutNeuron(int, NETRESULTS *);
	double RequestTemp(void);
	double RequestThresh(void);
	int RequestLayerSize(int);
	inline int GetAlive() { return Alive; }
	inline NETWORK *GetNext() { return next; }
	inline void SetNext(NETWORK *netptr) { next = netptr; }

private:
	int SetWeights(void);
};
#endif