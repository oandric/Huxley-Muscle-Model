#include <stdlib.h>
//#include <stdint.h>
#include<stdio.h>

int main(int argc, char *argv[])
{
	float *x, *n, *f, *v, *outX, *outN, *outF, *outV, *outFcon, *outVcon;
	float *x_curr, *n_curr, *f_curr, *v_curr;
	float *x_next, *n_next, *f_next, *v_next;
	float *v_con, *f_con;
	float step = 0.0156, poc = -23.4, dt = 0.001;
	float f1 = 43.3, g1 = 10.0, g2 = 209.0, h = 15.6, fx, gx;
	float sumX = 0, sumN = 0, sumF = 0, sumV = 0, sumFcon = 0, sumVcon = 0;
	int i, size = 8000000, iter = 0,  k = 0;
	struct timeval start, end;
	long mtime, seconds, useconds;

	x = malloc(size * sizeof(float));
	n = malloc(size * sizeof(float));
	f = malloc(size * sizeof(float));
	v = malloc(size * sizeof(float));

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


	if(!x || !n || !f || !v || !outX || !outN || !outF || !outV || !outFcon || !outVcon) {

			fprintf(stderr, "Failed to allocate memory for data I/O.\n");
			return 1;

		}
	printf("Generating data...\n");

	for(i = 0; i < size; i++){

		if(i % 8000 == 0)
			k = 0;

		x[i] = poc + step * k;
		n[i] = 0.0;
		v[i] = 107.4;

		fx = 0.0 + (0 <= x[i]) * (x[i] <= h) * f1 * x[i] / h;
		gx = (0 <= x[i]) * g1 * x[i] / h + (0 > x[i]) * g2;
		f[i] = fx * (1.0 - n[i]) - gx * n[i];
	
		k++;
	}

	printf("Calculating...\n");

	gettimeofday(&start, NULL);

	for(i = 0; i < size; i++){

		x_curr[i] = x[i] + v[i] * dt;
		n_curr[i] = n[i] + f[i] * dt;
		v_curr[i] = v[i];
		f_curr[i] = f[i];

	}

	while(iter++ < 10){

		for(i = 0; i < size; i++){

			x_next[i] = x[i] + 0.5 * (v_curr[i] + v[i]) * dt;
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

	for(i = 0; i < size; i++){

		outX[i] = x_curr[i];
		outN[i]	= n_curr[i];
		outF[i]	= f_curr[i];
		outV[i] = v_curr[i];

	}
	gettimeofday(&end, NULL);

	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = ((seconds) * 1000 + useconds / 1000.0);

	printf("Outputting data...\n");

	for(i = 0; i < size; i++){

		sumX = sumX + outX[i];
		sumN = sumN + outN[i];
		sumF = sumF + outF[i];
		sumV = sumV + outV[i];
		sumFcon = sumFcon + outFcon[i];
		sumVcon = sumVcon + outVcon[i];
		if(outFcon[i] != 0)
			printf("outFcon[%d] = %f\n", i, outFcon);
	}

	printf("sumX = %f\n", sumX);
	printf("sumN = %f\n", sumN);
	printf("sumF = %f\n", sumF);
	printf("sumV = %f\n", sumV);
	printf("sumFcon = %f\n", sumFcon);
	printf("sumVcon = %f\n", sumVcon);
 
	printf("Elapsed time: %ld milliseconds \n", mtime);

	return 1;

}
