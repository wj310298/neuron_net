#ifndef _BACKPROP_HPP
#define _BACKPROP_HPP
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
};
#endif