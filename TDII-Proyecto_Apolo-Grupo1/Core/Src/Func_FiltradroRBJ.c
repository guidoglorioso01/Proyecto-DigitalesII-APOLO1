/*
 * FilterRBJ.c
 *
 *  Created on: 5 nov. 2023
 *      Author: guido
 */

#include <Control_ProcesamientoAudio.h>


//#############################################################################
// Funciones de diseño
//#############################################################################

float32_t convertBwToOctaves(double fc_inf, double fc_sup) {
    // Calcula la diferencia en octavas
    return log2(fc_sup / fc_inf);
}

float32_t get_alpha(float32_t w0, float32_t Q) {
    return sin(w0) / (2 * Q);
}

float32_t get_A(float32_t dbGain) {
    return sqrt(pow(10, dbGain / 20)); // (para filtros peaking y shelving EQ únicamente)
}

float32_t get_w0(float32_t f0) {
	float32_t Fs = 44.1e3; // frecuencia de muestreo
    return 2 * M_PI * f0 / Fs;
}

float32_t get_alpha_fromBW(float32_t w0, float32_t BW) {
    return sin(w0) * sinh(log(2) / 2 * BW * w0 / sin(w0));
}


void low_shelf_pass(float32_t f0, float32_t dbGain, float32_t Q, float32_t* b, float32_t* a) {
	float32_t w0 = get_w0(f0);
    float32_t A = get_A(dbGain);
    float32_t alpha = get_alpha(w0, Q);

    float32_t b0 = A * ((A + 1) - (A - 1) * cos(w0) + 2 * sqrt(A) * alpha);
    float32_t b1 = 2 * A * ((A - 1) - (A + 1) * cos(w0));
    float32_t b2 = A * ((A + 1) - (A - 1) * cos(w0) - 2 * sqrt(A) * alpha);
    float32_t a0 = (A + 1) + (A - 1) * cos(w0) + 2 * sqrt(A) * alpha;
    float32_t a1 = -2 * ((A - 1) + (A + 1) * cos(w0));
    float32_t a2 = (A + 1) + (A - 1) * cos(w0) - 2 * sqrt(A) * alpha;

    b[0] = b0 / a0;
    b[1] = b1 / a0;
    b[2] = b2 / a0;
    //a[0] = 1;
    a[0] = -a1 / a0;
    a[1] = -a2 / a0;
}

void high_shelf_pass(float32_t f0, float32_t dbGain, float32_t Q, float32_t* b, float32_t* a) {
	float32_t w0 = get_w0(f0);
	float32_t A = get_A(dbGain);
	float32_t alpha = get_alpha(w0, Q);

	float32_t b0 = A * ((A + 1) + (A - 1) * cos(w0) + 2 * sqrt(A) * alpha);
	float32_t b1 = -2 * A * ((A - 1) + (A + 1) * cos(w0));
	float32_t b2 = A * ((A + 1) + (A - 1) * cos(w0) - 2 * sqrt(A) * alpha);
	float32_t a0 = (A + 1) - (A - 1) * cos(w0) + 2 * sqrt(A) * alpha;
	float32_t a1 = 2 * ((A - 1) - (A + 1) * cos(w0));
	float32_t a2 = (A + 1) - (A - 1) * cos(w0) - 2 * sqrt(A) * alpha;

    b[0] = b0 / a0;
    b[1] = b1 / a0;
    b[2] = b2 / a0;
    //a[0] = 1;
    a[0] = -a1 / a0;
    a[1] = -a2 / a0;
}


void peaking_filter(float32_t f0, float32_t dbGain, float32_t BW, float32_t* b, float32_t* a) {
    // the bandwidth in octave
	float32_t w0 = get_w0(f0);
	float32_t A = get_A(dbGain);
	float32_t alpha = get_alpha_fromBW(w0, BW);

	float32_t b0 = 1 + alpha * A;
	float32_t b1 = -2 * cos(w0);
	float32_t b2 = 1 - alpha * A;
	float32_t a0 = 1 + alpha / A;
	float32_t a1 = -2 * cos(w0);
	float32_t a2 = 1 - alpha / A;

    b[0] = b0 / a0;
    b[1] = b1 / a0;
    b[2] = b2 / a0;
    //a[0] = 1;
    a[0] = -a1 / a0;
    a[1] = -a2 / a0;
}

// Función para filtro de paso alto (HPF)
void high_pass(float32_t f0, float32_t Q, float32_t* b, float32_t* a) {
    float32_t w0 = get_w0(f0);
    float32_t alpha = get_alpha(w0, Q);
    float32_t b0 = (1 + cos(w0)) / 2;
    float32_t b1 = -(1 + cos(w0));
    float32_t b2 = (1 + cos(w0)) / 2;
    float32_t a0 = 1 + alpha;
    float32_t a1 = -2 * cos(w0);
    float32_t a2 = 1 - alpha;

    b[0] = b0 / a0;
    b[1] = b1 / a0;
    b[2] = b2 / a0;
    //a[0] = 1;
    a[0] = -a1 / a0;
    a[1] = -a2 / a0;
}

// Función para filtro de paso bajo (LPF)
void low_pass(float32_t f0, float32_t Q, float32_t* b, float32_t* a) {
    float32_t w0 = get_w0(f0);
    float32_t alpha = get_alpha(w0, Q);
    float32_t b0 = (1 - cos(w0)) / 2;
    float32_t b1 = 1 - cos(w0);
    float32_t b2 = (1 - cos(w0)) / 2;
    float32_t a0 = 1 + alpha;
    float32_t a1 = -2 * cos(w0);
    float32_t a2 = 1 - alpha;

    b[0] = b0 / a0;
    b[1] = b1 / a0;
    b[2] = b2 / a0;
    //a[0] = 1;
    a[0] = -a1 / a0;
    a[1] = -a2 / a0;
}

// Función para filtro de banda (BPF)
void band_pass(float32_t f0, float32_t dbGain, float32_t BW, float32_t* b, float32_t* a) {
	float32_t A = get_A(dbGain);
	float32_t w0 = get_w0(f0);
	float32_t alpha = get_alpha_fromBW(w0, BW);

	float32_t b0 = sin(w0) / 2;
	float32_t b1 = 0;
	float32_t b2 = -sin(w0) / 2;
	float32_t a0 = 1 + alpha;
	float32_t a1 = -2 * cos(w0);
	float32_t a2 = 1 - alpha;

	b[0] = A * b0 / a0;
	b[1] = A * b1 / a0;
	b[2] = A * b2 / a0;
	//a[0] = 1;
	a[0] = -a1 / a0;
	a[1] = -a2 / a0;
}

//#############################################################################
//Diseño filtros
//#############################################################################

void designFilter(float32_t gainsdB[7], float32_t Q_BW[7], float32_t *sos,type_data_filter type) {
//Para q32:
//	The coefficients are stored in the array pCoeffs in the following order:
//	    {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
//Para q15
//	The coefficients are stored in the array pCoeffs in the following order:
//	    {b10, 0, b11, b12, a11, a12, b20, 0, b21, b22, a21, a22, ...}

	float32_t b[3], a[2];
    int i=0;
    if(type == Q32){
		low_shelf_pass(100, gainsdB[0], Q_BW[0], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

		peaking_filter(200, gainsdB[1], Q_BW[1], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

		peaking_filter(500, gainsdB[2], Q_BW[2], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

		peaking_filter(1200, gainsdB[3], Q_BW[3], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

		peaking_filter(4000, gainsdB[4], Q_BW[4], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

		peaking_filter(8000, gainsdB[5], Q_BW[5], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

		high_shelf_pass(13000, gainsdB[6], Q_BW[6], b, a);
		for(int i2=0;i2<3;i2++,i++) sos[i] = b[i2];
		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];
    }
    if(type == Q15){

    	low_shelf_pass(100, gainsdB[0], Q_BW[0], b, a);
 		sos[i++] = b[0];
 		sos[i++] = 0;
 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

// 		peaking_filter(200, gainsdB[1], Q_BW[1], b, a);
// 		sos[i++] = b[0];
// 		sos[i++] = 0;
// 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
// 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

 		peaking_filter(500, gainsdB[2], Q_BW[2], b, a);
 		sos[i++] = b[0];
 		sos[i++] = 0;
 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

 		peaking_filter(1200, gainsdB[3], Q_BW[3], b, a);
 		sos[i++] = b[0];
 		sos[i++] = 0;
 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

// 		peaking_filter(4000, gainsdB[4], Q_BW[4], b, a);
// 		sos[i++] = b[0];
// 		sos[i++] = 0;
// 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
// 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

 		peaking_filter(8000, gainsdB[5], Q_BW[5], b, a);
 		sos[i++] = b[0];
 		sos[i++] = 0;
 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];

 		high_shelf_pass(13000, gainsdB[6], Q_BW[6], b, a);
 		sos[i++] = b[0];
 		sos[i++] = 0;
 		for(int i2=1;i2<3;i2++,i++) sos[i] = b[i2];
 		for(int i2=0;i2<2;i2++,i++) sos[i] = a[i2];
     }

}


void designCrossover(float32_t *buff,uint8_t Type){
	float32_t b[3], a[2];

	switch(Type){
		case TYPE_LP1:
			low_pass(200, 0.5, b, a);
		break;
		case TYPE_LP2:
			low_pass(500, 0.5, b, a);
		break;
		case TYPE_BP1:
			band_pass(3100,30,convertBwToOctaves(200, 6000), b, a);
		break;
		case TYPE_BP2:
			band_pass(3750,23,convertBwToOctaves(500, 7000), b, a);
		break;
		case TYPE_HP1:
			high_pass(8000,0.5, b, a);
		break;
		case TYPE_HP2:
			high_pass(10000, 0.5, b, a);
		break;
		default:
		case TYPE_FLAT:
			return;
		break;
		}

	buff[0]= b[0];
	buff[1]= 0 ;
	buff[2]= b[1];
	buff[3]= b[2];
	buff[4]= a[0];
	buff[5]= a[1];

}
