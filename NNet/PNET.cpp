#include "PNET.hpp"
#include <stdio.h>
#include <math.h>

//*********************************************************************************************************************************************************************************************
// METHODS FOR CLASS NETFILEDATA                                                                                                                                                              *
//*********************************************************************************************************************************************************************************************
NETFILEDATA::NETFILEDATA() {
	int i;
	for (i = 0; i < MAXLAYERS - 1; ++i) {
		weights[i] = NULL;
	}
}

int NETFILEDATA::SetupNet(char *wgt_file_name) {
	FILE *wgt_file_ptr;
	double AWeight;
	if ((wgt_file_ptr = fopen(wgt_file_name, "r")) == NULL)
		return 1;
	fscanf(wgt_file_ptr, "%lg", &threshold);				//Read in threshold value
	fscanf(wgt_file_ptr, "%lg", &temperature);				//Read in temperature value
	fscanf(wgt_file_ptr, "%d", &Nlayers);					//Read in # of layers
	for (int j = 0; j < MAXLAYERS; ++j)
		neurons[j] = 0;										//Initialize array to zero
	for (int i = 0; i < Nlayers; ++i)						//Traverse src lyr nodes
		fscanf(wgt_file_ptr, "%d", &neurons[i]);
	for (int lyr = 1; lyr < Nlayers; ++lyr) {
		for (int dn = 0; dn < neurons[lyr]; ++dn) {
			for (int sn = 0; sn <= neurons[lyr - 1]; ++sn) {
				fscanf(wgt_file_ptr, "%lg", &AWeight);
				ADDweights(lyr - 1, dn, sn, AWeight);		//Add new weight to the net
			}
		}
	}
	fclose(wgt_file_ptr);
	return 0;
}

void NETFILEDATA::ADDweights(int l, int d, int s, double w) {
	WEIGHTIMAGE *WI = weights[l],
		*Wnew = new WEIGHTIMAGE,
		*cursor,
		*trailer;
	Wnew->data    = w;
	Wnew->dneuron = d;
	Wnew->sneuron = s;
	Wnew->next    = NULL;
	if (WI) {
		cursor  = WI;
		trailer = NULL;
		while (cursor) {
			trailer = cursor;
			cursor = cursor->next;
		}
		trailer->next = Wnew;
	}
	else
		weights[l] = Wnew;
}

double NETFILEDATA::GETweights(int l, int d, int s) {
	WEIGHTIMAGE *WI = weights[l];

	while (WI) {
		if ((WI->sneuron == s) && (WI->dneuron == d)) {
			status = 0;
			return WI->data;
		}
		WI = WI->next;
	}
	status = 1;
	return 0.0;
}

//*********************************************************************************************************************************************************************************************
// METHODS FOR CLASS NEURON                                                                                                                                                                   *
//*********************************************************************************************************************************************************************************************
void NEURON::SetWeight(double Wght, NEURON *SrcPtr) {
	WEIGHT *W = new WEIGHT(Wght, SrcPtr);
	if (weight1 == NULL) {
		weight1 = weightL = W;
	}
	else {
		weightL->SetNext(W);
		weightL = W;
	}
}

void NEURON::Calc(void) {
	NEURON *Nptr;
	WEIGHT *Wptr = weight1;
	double NET = 0.0,
		PLNout,
		Weight;

	if (!BiasFlg) {
		while (Wptr) {
			Weight = Wptr->GetWeight();
			Nptr = Wptr->GetSRCNeuron();		//Get weight between prev/curr layer
			PLNout = Nptr->GetOut();			//Get the previous layer output(out)
			NET += Weight * PLNout;				//Sum(weight*out) over the curr layer
			Wptr = Wptr->GetNext();				//Get the next weight in the weight list
		}
		//Calculate a neuron output using a sigmoid
		out = 1 / (1 + exp(-(NET + threshold) / temperature));
	}
	else {
		out = 1.0;								//force output on for bias neuron
	}
}

//*********************************************************************************************************************************************************************************************
// METHODS FOR CLASS LAYER                                                                                                                                                                    *
//*********************************************************************************************************************************************************************************************
LAYER::LAYER(int layer_id, NETFILEDATA *netdata) {
	NEURON *Nptr;
	LayerID = layer_id;
	Neuron1 = NeuronL = NULL;
	next = NULL;
	//Get # of  neurons in layer #layer_id
	Ncount = netdata->GetLayerSize(layer_id);
	for (unsigned int i = 0; i <= Ncount; ++i) {
		if (i == Ncount) {
			Nptr = new NEURON(i, TRUE);			//This is a bias Neuron
		}
		else {
			Nptr = new NEURON(i);
		}
		if (Neuron1 == NULL) {
			Neuron1 = NeuronL = Nptr;
		}
		else {
			NeuronL->SetNext(Nptr);
			NeuronL = Nptr;
		}
	}
}

void LAYER::Calc(int out_layer) {
	NEURON *Nptr = Neuron1;
	while (Nptr) {
		Nptr->Calc();
		Nptr = Nptr->GetNext();
	}
}

int LAYER::SetWeight(NEURON *PrevNeuron, NETFILEDATA *netdata) {
	NEURON *CurNeuron = Neuron1, *PrevPtr;
	double ZWeight = 0.0;
	int curx = 0,
		prevx,
		status = 0;
	while (CurNeuron != NULL) {
		if (!CurNeuron->IsBias()) {				//Bias neurons don't have incoming wgts
			PrevPtr = PrevNeuron;
			prevx = 0;
			while (PrevPtr) {
				ZWeight = netdata->GetWeight(LayerID, curx, prevx++);
				status = netdata->GetStatus();
				if (status > 0)
					return status;
				CurNeuron->SetWeight(ZWeight, PrevPtr);
				PrevPtr = PrevPtr->GetNext();
			}
		}
		++curx;
		CurNeuron = CurNeuron->GetNext();
	}
	return status;
}

//*********************************************************************************************************************************************************************************************
// METHODS FOR CLASS NETWORK                                                                                                                                                                  *
//*********************************************************************************************************************************************************************************************
int NETWORK::Setup(char *wgt_file_name) {
	LAYER *Lptr;
	NEURON N;
	int status = 0;
	//Set Up network using info in the weight file
	status = netdata.SetupNet(wgt_file_name);
	if (status > 0)
		return 1;
	Nlayers = netdata.GetNlayers();
	N.SetTemperature(netdata.GetTemp());
	N.SetThreshold(netdata.GetThreash());
	for (int i = 0; i < Nlayers; ++i) {
		Lptr = new LAYER(i, &netdata);
		if (i == 0) {
			INlayer = OUTlayer = Lptr;
		}
		else {
			OUTlayer->SetNext(Lptr);
			OUTlayer = Lptr;
		}
	}
	status = SetWeights();
	if (status > 0)
		return 2;
	Alive = 1;
	return 0;
}

int NETWORK::SetWeights(void) {
	LAYER *L1ptr = INlayer,
		*L2ptr = INlayer->GetNext();
	int status = 0;
	while (L2ptr) {
		status = L2ptr->SetWeight(L1ptr->GetFirstNeuron(), &netdata);
		L1ptr = L1ptr->GetNext();
		L2ptr = L2ptr->GetNext();
	}
	return status;
}

void NETWORK::ApplyVector(unsigned char *InVecPtr, int size) {
	LAYER *Lptr = INlayer;
	NEURON *Nptr = Lptr->GetFirstNeuron();
	double value;
	unsigned char mask;
	while (Nptr && (!Nptr->IsBias())) {
		for (int i = 0; i < size; ++i) {
			mask = 0x80;
			for (int j = 0; j < 8; ++j) {
				if ((InVecPtr[i] & mask) != 0)
					value = NEURONHIGH;
				else
					value = NEURONLOW;
				Nptr->SetOut(value);
				Nptr = Nptr->GetNext();
				mask = mask >> 1;
			}
		}
	}
}

void NETWORK::RunNetWork() {
	LAYER *Lptr = INlayer->GetNext();
	int out_layer = 0;
	while (Lptr) {
		if (Lptr->GetNext() == NULL)
			out_layer = 1;
		Lptr->Calc(out_layer);
		Lptr = Lptr->GetNext();
	}
}

int NETWORK::RequestNthOutNeuron(int neuron_num, NETRESULTS *results) {
	if (Alive == 0)
		return 2;
	NEURON *Nptr = OUTlayer->GetFirstNeuron();
	while (Nptr) {
		if (neuron_num == Nptr->GetId()) {
			results->index = neuron_num;
			results->value = Nptr->GetOut();
			results->character = '1';
			return 0;
		}
		Nptr = Nptr->GetNext();
	}
	return 1;
}

double NETWORK::RequestTemp(void) {
	if (Alive == 0)
		return -1.0;
	else
		return netdata.GetTemp();
}

double NETWORK::RequestThresh(void) {
	if (Alive == 0)
		return -1.0;
	else
		return netdata.GetThreash();
}

int NETWORK::RequestLayerSize(int layer_num) {
	if (Alive == 0)
		return 0;
	else if (layer_num >= MAXLAYERS)
		return 0;
	else
		return netdata.GetLayerSize(layer_num);
}