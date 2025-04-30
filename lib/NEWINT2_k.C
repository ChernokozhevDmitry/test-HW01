#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "global_variable.h"
#include "NEWINT2_k.H"

int monlan4(char* ima1, char* ima2, char* ima3, char* ima4, char* ima5,
	int teps, double eps, int otb, double rmin, double cont,
	double xsetndu, double xsetkdu, double ysetndu, double ysetkdu, int nxset, int nyset,
	double osred, int ITERAZ,
	char* maska, double kodmask, PROGRESS progr, int thread_id)
{
// for demonstration multithreading from c++ project
	if (thread_id == -1)
	{
		printf("thread%d: ima1 = %s; ima2 = %s; ima3 = %s; ima4 = %s; xsetndu = %.0f; xsetkdu = %.0f; ysetndu = %.0f; ysetkdu = %.0f; nxset = %d; nyset = %d; progress = %s\n",
				thread_id,ima1,      ima2,      ima3,      ima4,      xsetndu,      xsetkdu,      ysetndu,      ysetkdu,      nxset,      nyset,      (progr) ? "true" : "0");
	}
	else
	{
		printf("thread%d: ima1 = %s\t ima2 = %s\t ima3 = %s\t ima4 = %s\t ysetndu = %.0f\t ysetkdu = %.0f\t nyset = %d\t progress = %s\n",
				thread_id,ima1,      ima2,      ima3,      ima4,      ysetndu,      ysetkdu,      nyset,      (progr) ? "true" : "0");
	}
	
	surf2(nxset, nyset, ima4, thread_id);
	
	return 0;
}

int  monlan3(char *ima1, char* ima2, char* ima3, char* ima4, char* ima5,
	int teps, double eps, int otb, double rmin, double cont, double xsetndu, double xsetkdu,
	double ysetndu, double ysetkdu, int nxset, int nyset, double osred, int ITERAZ,
	char* maska, double kodmask, PROGRESS progr)
{
	int thread_id = -1; // if singlethreading
	return monlan4(ima1, ima2, ima3, ima4, ima5,
	           teps, eps, otb, rmin, cont,
			   xsetndu, xsetkdu, ysetndu, ysetkdu, nxset, nyset,
			   osred, ITERAZ, maska, kodmask, progr, thread_id);
}


 int surf2(int long nx, int long ny, char* im, int thread_id)
{
 	int long i;
	int long j;
	int long k = 0;

	FILE * f1;

	float* V;

	if (thread_id == -1) // if singlethreading
	{
		V = (float*)malloc(nx * sizeof(float));
		if (V == NULL) return -1;

		if (im) f1 = fopen(im, "wb");

		for (j = 0; j < ny; j++)
		{
		 	for (i = 0; i < nx; i++)
			{
				V[i] = static_cast<float>(777.777); // for example only
				++k;				
		    }
			if (im)
			{
				fwrite("FROM-SINGLE-THREAD", 18, 1, f1); // for example only
				fwrite(&V[0], sizeof(float)*nx, 1, f1); // for example only
			}	
		}	

		if (im) fclose(f1);

		free(V);
	} 
	else // if multithreading
	{
		for (j = 0; j < ny; j++)
		{
			for (i = 0; i < nx; i++)
			{
				V_global[thread_id*nx*ny + j * nx + i] = static_cast<float>(777.777); // for example only
				++k;
			}
		}
	}
	return(0);
}
