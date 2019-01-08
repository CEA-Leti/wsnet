/**
 *  \file   Intermodulation.c
 *  \brief  Intermodulation
 *  \author Amine DIDIOUI
 *  \date   2011
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
  "Intermodulation",
  "Amine DIDIOUI",
  "0.1",
   MODELTYPE_INTERMODULATION
};


/* ************************************************** */
/* ************************************************** */
struct entitydata {
  double IIP3;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
	return 0;
}

int destroy(call_t *to) {
    return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
	return 0;
}

int unbind(call_t *to) {
	return 0;
}


/* ************************************************** */
/* ************************************************** */
double intermod(call_t *to, call_t *from, int channel0, int channel1, int channel2) {
	//struct entitydata *entitydata = get_class_private_data(to);
	double tmpFactor = 0;
	int tmp = 0;
	int i=0,j=0,k=0;
	double adj,altern1,altern2,IMD3,IIP3;

	IIP3 = -24;
	//printf("\nIIP3 %lf\n",entitydata->IIP3);

	IMD3 = dBm2mW(-2*IIP3);

	adj = IMD3*dBm2mW(-49);
	altern1 = IMD3*dBm2mW(-54);
	altern2 = IMD3*dBm2mW(-55);

	for(k = 0; k <= channel0; k++)
	{
		for(i = 0; i <= channel1; i++)
		{
			for(j = 0; j <= channel2; j++)
			{
				if(i == j) {
					tmpFactor = 0;
				} else {
					tmp = abs(2*i-j-k);
					if(tmp == 0)
						tmpFactor = IMD3;
					else if(tmp == 1)
						tmpFactor = adj;
					else if(tmp == 2)
						tmpFactor = altern1;
					else if(tmp >= 3)
						tmpFactor = altern2;
				}
			}
		}
	}

	return tmpFactor;
}


/* ************************************************** */
/* ************************************************** */
intermodulation_methods_t methods = {intermod};
