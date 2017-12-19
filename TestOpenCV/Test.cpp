#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <cmath>
#include "cxcore.h"
#include <stdio.h>

void load(int n, float w[1000], float x[1000][14]) {
	FILE  *fp;
	int i = 0;

	fp = fopen("SV_coef.txt", "rb");
	for (i = 0; i < n; i++) {
		fscanf(fp, "%f", &w[i]);
		printf( "%f\n", w[i]);
	}
	fclose(fp);

	fp = fopen("SVs.txt", "rb");
	for (i = 0; i < n; i++) {
		for (int j = 0; j < 14; j++)
		{
			fscanf(fp, "%f", &x[i][j]);
			printf( "%f ", x[i][j]);
		}
		printf( "\n" );
	}
	fclose(fp);
}

int main(int argc, char** argv) {
	float b = 0;
	float gamma = 0;
	int n = 0;
	float w[1000] = {0};
	float x[1000][14] = {0};
	FILE* fp;

	fp = fopen("Parameters.txt", "rb");
	fscanf(fp, "%f", &b);
	fscanf(fp, "%f", &gamma);
	fscanf(fp, "%d", &n);
	fclose(fp);
	load(n, w, x);
	printf("%f\n", b);
	printf("%d\n", n);
	printf("%f\n", gamma);
}
