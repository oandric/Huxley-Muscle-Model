#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

void generateInputData(int size, float *x, float *n, float poc, float step);

int main(void) {

	float *x, *n, *outX, *outN, *outFcon, *outVcon, *outF, *outV;
	//float *expectedOutX, *expectedOutN, *expectedOutFcon, *expectedOutVcon;
	float step = 0.0156, poc = -23.4, dt = 0.001;
	float f1 = 43.3, g1 = 10.0, g2 = 209.0, h = 15.6, v = 107.4; //, fx, gx;
	float sumX = 0, sumN = 0, sumFcon = 0, sumVcon = 0, sumV = 0, sumF = 0;
	int i;
	int size = 8000000;

	struct timeval start, end;
	long mtime, seconds, useconds;

	x = malloc(size * sizeof(float));
	n = malloc(size * sizeof(float));
	outX = malloc(size * sizeof(float));
	outN = malloc(size * sizeof(float));
	outF = malloc(size * sizeof(float));
	outV = malloc(size * sizeof(float));
	outFcon = malloc(size * sizeof(float));
	outVcon = malloc(size * sizeof(float));

	//advanced static slic

	max_file_t *myMaxFile = HuxleyMuscleModel_init();
	max_engine_t *myDFE = max_load(myMaxFile, "local:*");

	HuxleyMuscleModel_actions_t actions;
	actions.param_size = size;
	actions.param_dt = dt;
	actions.param_v = v;
	actions.param_f1 = f1;
	actions.param_g1 = g1;
	actions.param_g2 = g2;
	actions.param_h = h;
	actions.instream_X = x;
	actions.instream_N = n;
	actions.outstream_outputX = outX;
	actions.outstream_outputN = outN;
	actions.outstream_outputF = outF;
	actions.outstream_outputV = outV;
	actions.outstream_outputFcon = outFcon;
	actions.outstream_outputVcon = outVcon;

	printf("Generating data to send to FPGA.\n");

	generateInputData(size, x, n, poc, step);

	printf("Streaming data to/from FPGA.\n");
	gettimeofday(&start, NULL);

	/*HuxleyMuscleModel(size, dt, g1, g2, h, v, f1, n, x, outFcon, outN, outVcon,
	 outX);*/
	HuxleyMuscleModel_run(myDFE, &actions);

	for (i = 0; i < size; i++) {
		sumX = sumX + outX[i];
		sumN = sumN + outN[i];
		sumV = sumV + outV[i];
		sumF = sumF + outF[i];
		sumFcon = sumFcon + outFcon[i];
		sumVcon = sumVcon + outVcon[i];

	}

	gettimeofday(&end, NULL);

	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds / 1000.0);

	printf("Outputting data read from FPGA.\n");

	printf("sumX = %lf\n", sumX);
	printf("sumN = %lf\n", sumN);
	printf("sumF = %lf\n", sumF);
	printf("sumV = %lf\n", sumV);
	printf("sumFcon = %lf\n", sumFcon);
	printf("sumVcon = %lf\n", sumVcon);
	printf("Elapsed time: %ld milliseconds \n", mtime);

	free(x);
	free(n);
	free(outN);
	free(outX);
	free(outFcon);
	free(outVcon);

	return 0;
}
void generateInputData(int size, float *x, float *n, float poc, float step) {
	int i, j;
	j = 0;
	for (i = 0; i < size; i++) {
		if (i % 8000 == 0)
			j = 0;
		x[i] = poc + step * j;
		n[i] = 0.0;
		j++;
	}

}

