#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Maxfiles.h"
#include "MaxSLiCInterface.h"

int size = 80000;
float step = 0.0156, poc = -23.4, dt = 0.001;
float f1 = 43.3, g1 = 10.0, g2 = 209.0, h = 15.6, v=107.4;

int check(float resultsDFE[], float resultsCPU[], int resultSize);
void generateInputData(int size, float *x, float *n, float poc, float step);
void huxleyMuscleModelCPU(float resultsDFE[]);
void huxleyMuscleModelDFE(float resultsCPU[]);

int main(void) {

	int resultSize = 6;
	int status;
	int i=0;

	float resultsCPU[6];
	float resultsDFE[6];

	for(i=0; i<resultSize; i++){
		resultsCPU[i] = 0;
		resultsDFE[i] = 0;
	}

	huxleyMuscleModelDFE(resultsDFE);
	huxleyMuscleModelCPU(resultsCPU);

	printf("Finished!\n");

	status = check(resultsDFE, resultsCPU, resultSize);

	if(status==0)
		printf("Test passed successfully!\n");
	else
		printf("Test failed %d times!\n", status);

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
void huxleyMuscleModelDFE(float resultsDFE[]){

	float *x, *n, *outX, *outN, *outFcon, *outVcon, *outF, *outV;
	//float *expectedOutX, *expectedOutN, *expectedOutFcon, *expectedOutVcon;
	float sumX = 0, sumN = 0, sumFcon = 0, sumVcon = 0, sumV = 0, sumF = 0;
	int i;

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

	printf("Outputting data from FPGA.\n");

	printf("DFE elapsed time: %ld milliseconds \n", mtime);

	resultsDFE[0] = sumX;
    resultsDFE[1] = sumN;
	resultsDFE[2] = sumV;
    resultsDFE[3] = sumF;
    resultsDFE[4] = sumFcon;
    resultsDFE[5] = sumVcon;

	free(x);
	free(n);
	free(outN);
	free(outX);
	free(outFcon);
	free(outVcon);

}

void huxleyMuscleModelCPU(float resultsCPU[]) {
	float *x, *n, *f, *outX, *outN, *outF, *outV, *outFcon, *outVcon;
	float *x_curr, *n_curr, *f_curr, *v_curr;
	float *x_next, *n_next, *f_next, *v_next;
	float *v_con, *f_con;

	float  fx, gx;
	float sumX = 0, sumN = 0, sumF = 0, sumV = 0, sumFcon = 0, sumVcon = 0;
	int i, iter = 0;

	struct timeval start, end;
	long mtime, seconds, useconds;

	x = malloc(size * sizeof(float));
	n = malloc(size * sizeof(float));
	f = malloc(size * sizeof(float));

	x_curr = malloc(size * sizeof(float));
	n_curr = malloc(size * sizeof(float));
	f_curr = malloc(size * sizeof(float));
	v_curr = malloc(size * sizeof(float));

	x_next = malloc(size * sizeof(float));
	n_next = malloc(size * sizeof(float));
	f_next = malloc(size * sizeof(float));
	v_next = malloc(size * sizeof(float));

	v_con = malloc(size * sizeof(float));
	f_con = malloc(size * sizeof(float));

	outX = malloc(size * sizeof(float));
	outN = malloc(size * sizeof(float));
	outF = malloc(size * sizeof(float));
	outV = malloc(size * sizeof(float));
	outFcon = malloc(size * sizeof(float));
	outVcon = malloc(size * sizeof(float));

	if (!x || !n || !f || !v || !outX || !outN || !outF || !outV || !outFcon
			|| !outVcon) {

		fprintf(stderr, "Failed to allocate memory for data I/O.\n");
		exit(1);

	}

	generateInputData(size, x, n, poc, step);

	printf("CPU calculating...\n");

	gettimeofday(&start, NULL);

	for (i = 0; i < size; i++) {

		x_curr[i] = x[i] + v * dt;
		n_curr[i] = n[i] + f[i] * dt;
		v_curr[i] = v;
		f_curr[i] = f[i];

		fx = 0.0 + (0 <= x[i]) * (x[i] <= h) * f1 * x[i] / h;
		gx = (0 <= x[i]) * g1 * x[i] / h + (0 > x[i]) * g2;
		f[i] = fx * (1.0 - n[i]) - gx * n[i];

	}

	while (iter++ < 10) {

		for (i = 0; i < size; i++) {

			x_next[i] = x[i] + 0.5 * (v_curr[i] + v) * dt;
			v_next[i] = (x_next[i] - x[i]) / dt;

			// calculating fun_F
			fx = 0.0 + (0 <= x_next[i]) * (x_next[i] <= h) * f1 * x_next[i] / h;
			gx = (0 <= x_next[i]) * g1 * x_next[i] / h + (0 > x_next[i]) * g2;
			f_next[i] = fx * (1.0 - n_curr[i]) - gx * n_curr[i];

			n_next[i] = n[i] + 0.5 * (f_next[i] + f[i]) * dt;

			v_con[i] = v_next[i] - v_curr[i];
			f_con[i] = f_next[i] - f_curr[i];

			x_curr[i] = x_next[i];
			n_curr[i] = n_next[i];
			f_curr[i] = f_next[i];
			v_curr[i] = v_next[i];

		}

	}

	for (i = 0; i < size; i++) {

		outX[i] = x_curr[i];
		outN[i] = n_curr[i];
		outF[i] = f_curr[i];
		outV[i] = v_curr[i];

	}
	gettimeofday(&end, NULL);

	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds / 1000.0);

	printf("Outputting data...\n");

	for (i = 0; i < size; i++) {

		sumX = sumX + outX[i];
		sumN = sumN + outN[i];
		sumF = sumF + outF[i];
		sumV = sumV + outV[i];
		sumFcon = sumFcon + outFcon[i];
		sumVcon = sumVcon + outVcon[i];
	}

	printf("CPU elapsed time: %ld milliseconds \n", mtime);

	resultsCPU[0] = sumX;
	resultsCPU[1] = sumN;
	resultsCPU[2] = sumV;
	resultsCPU[3] = sumF;
	resultsCPU[4] = sumFcon;
	resultsCPU[5] = sumVcon;

}

int check(float resultsDFE[], float resultsCPU[], int resultSize){
	int status = 0;
	int i;
	for (i = 0; i < resultSize; ++i) {
		if(resultsDFE[i] != resultsCPU[i])
			status++;
	}
	return status;
}


