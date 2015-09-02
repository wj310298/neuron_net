#include "PNET.hpp"
#include "stdio.h"

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
	fscanf(wgt_file_ptr, "%lg", &threshold);
	fscanf(wgt_file_ptr, "%lg", &temperature);
	fscanf(wgt_file_ptr, "%d", &Nlayers);
	for (int j = 0; j < MAXLAYERS; ++j)
		neurons[j] = 0;
	for (int i = 0; i < Nlayers; ++i)
		fscanf(wgt_file_ptr, "%d", &neurons[i]);
}