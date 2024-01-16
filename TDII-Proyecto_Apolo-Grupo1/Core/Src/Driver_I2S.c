/*
 * I2CDriver.c
 *
 *  Created on: 14 dic. 2023
 *      Author: Nic
 */
#include <Driver_I2S.h>

//#############################################################################
//Handlers perifericos
//#############################################################################
extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;
//extern SPI_HandleTypeDef hspi1;
//extern TIM_HandleTypeDef htim1;

//#############################################################################
//Double Buffering
//#############################################################################
//Buffers de entrada
int16_t bufferIn[PINPONG_BUFF_SIZE];
static volatile int16_t *ptrDmaIn = &bufferIn[0];
static volatile int16_t *ptrProcessIn = &bufferIn[PINPONG_BUFF_SIZE/2];

//Buffers de Salida AB
int16_t bufferOut_AB[PINPONG_BUFF_SIZE] ;//= {0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942, 0, 0, 942, 942, 1883, 1883, 2823, 2823, 3759, 3759, 4693, 4693, 5621, 5621, 6544, 6544, 7460, 7460, 8369, 8369, 9270, 9270, 10162, 10162, 11043, 11043, 11914, 11914, 12773, 12773, 13619, 13619, 14452, 14452, 15271, 15271, 16074, 16074, 16862, 16862, 17633, 17633, 18387, 18387, 19122, 19122, 19839, 19839, 20536, 20536, 21213, 21213, 21869, 21869, 22503, 22503, 23115, 23115, 23704, 23704, 24270, 24270, 24812, 24812, 25329, 25329, 25822, 25822, 26289, 26289, 26730, 26730, 27144, 27144, 27532, 27532, 27893, 27893, 28226, 28226, 28531, 28531, 28808, 28808, 29057, 29057, 29277, 29277, 29468, 29468, 29630, 29630, 29763, 29763, 29866, 29866, 29940, 29940, 29985, 29985, 30000, 30000, 29985, 29985, 29940, 29940, 29866, 29866, 29763, 29763, 29630, 29630, 29468, 29468, 29277, 29277, 29057, 29057, 28808, 28808, 28531, 28531, 28226, 28226, 27893, 27893, 27532, 27532, 27144, 27144, 26730, 26730, 26289, 26289, 25822, 25822, 25329, 25329, 24812, 24812, 24270, 24270, 23704, 23704, 23115, 23115, 22503, 22503, 21869, 21869, 21213, 21213, 20536, 20536, 19839, 19839, 19122, 19122, 18387, 18387, 17633, 17633, 16862, 16862, 16074, 16074, 15271, 15271, 14452, 14452, 13619, 13619, 12773, 12773, 11914, 11914, 11043, 11043, 10162, 10162, 9270, 9270, 8369, 8369, 7460, 7460, 6544, 6544, 5621, 5621, 4693, 4693, 3759, 3759, 2823, 2823, 1883, 1883, 942, 942, 0, 0, -942, -942, -1883, -1883, -2823, -2823, -3759, -3759, -4693, -4693, -5621, -5621, -6544, -6544, -7460, -7460, -8369, -8369, -9270, -9270, -10162, -10162, -11043, -11043, -11914, -11914, -12773, -12773, -13619, -13619, -14452, -14452, -15271, -15271, -16074, -16074, -16862, -16862, -17633, -17633, -18387, -18387, -19122, -19122, -19839, -19839, -20536, -20536, -21213, -21213, -21869, -21869, -22503, -22503, -23115, -23115, -23704, -23704, -24270, -24270, -24812, -24812, -25329, -25329, -25822, -25822, -26289, -26289, -26730, -26730, -27144, -27144, -27532, -27532, -27893, -27893, -28226, -28226, -28531, -28531, -28808, -28808, -29057, -29057, -29277, -29277, -29468, -29468, -29630, -29630, -29763, -29763, -29866, -29866, -29940, -29940, -29985, -29985, -30000, -30000, -29985, -29985, -29940, -29940, -29866, -29866, -29763, -29763, -29630, -29630, -29468, -29468, -29277, -29277, -29057, -29057, -28808, -28808, -28531, -28531, -28226, -28226, -27893, -27893, -27532, -27532, -27144, -27144, -26730, -26730, -26289, -26289, -25822, -25822, -25329, -25329, -24812, -24812, -24270, -24270, -23704, -23704, -23115, -23115, -22503, -22503, -21869, -21869, -21213, -21213, -20536, -20536, -19839, -19839, -19122, -19122, -18387, -18387, -17633, -17633, -16862, -16862, -16074, -16074, -15271, -15271, -14452, -14452, -13619, -13619, -12773, -12773, -11914, -11914, -11043, -11043, -10162, -10162, -9270, -9270, -8369, -8369, -7460, -7460, -6544, -6544, -5621, -5621, -4693, -4693, -3759, -3759, -2823, -2823, -1883, -1883, -942, -942};
static volatile int16_t *ptrProcessOutAB = &bufferOut_AB[PINPONG_BUFF_SIZE/2];
static volatile int16_t *ptrDmaOutAB = &bufferOut_AB[0];

////Buffers Salida CD
//int16_t bufferOut1_CD[BUFF_SIZE];
//int16_t bufferOut2_CD[BUFF_SIZE];
//int16_t *ptrProcessOutCD = bufferOut1_CD;
//int16_t *ptrDmaOutCD = bufferOut2_CD;

//#############################################################################
//Semaforos
//#############################################################################
SemaphoreHandle_t semRx_I2S;
SemaphoreHandle_t semTxAB_I2S;
SemaphoreHandle_t semTxCD_I2S;
SemaphoreHandle_t semDataReadyToSendDMA;
SemaphoreHandle_t semDataReadyToProc;

//#############################################################################
//Init
//#############################################################################
void initI2SDriver() {
	semRx_I2S = xSemaphoreCreateBinary();
	semTxAB_I2S = xSemaphoreCreateBinary();
	semTxCD_I2S = xSemaphoreCreateBinary();
	semDataReadyToSendDMA = xSemaphoreCreateBinary();
	semDataReadyToProc = xSemaphoreCreateBinary();

	xSemaphoreGive(semRx_I2S);
	xSemaphoreGive(semTxAB_I2S);
	xSemaphoreGive(semTxCD_I2S);
	xSemaphoreGive(semDataReadyToSendDMA);
	xSemaphoreTake(semDataReadyToProc, 0);

	//HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4);

	HAL_I2S_Transmit_DMA(&hi2s3,(uint16_t *) ptrDmaOutAB, PINPONG_BUFF_SIZE);

	// Busco un flanco ascendente (si se usa como Master al ST entonces se comentan estas lineas)
	//while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_RESET);    //use GPIO_PIN_SET for the other I2S mode
	//while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_SET);    //use GPIO_PIN_RESET for the other I2S mode   <<<<<<<< THIS

	HAL_I2S_Receive_DMA(&hi2s2,(uint16_t *) ptrDmaIn, PINPONG_BUFF_SIZE);
	//HAL_I2S_DMAPause(&hi2s2);
	//HAL_I2S_DMAResume(&hi2s2);
;}


//#############################################################################
//Direccionamiento Double Buffering
//#############################################################################
//void switchBufferIn() {
//	if(ptrProcessIn==bufferInA) {
//		ptrProcessIn = bufferInB;
//		ptrDmaIn = bufferInA;
//	} else {
//		ptrProcessIn = bufferInA;
//		ptrDmaIn = bufferInB;
//	}
//}
//
//void switchBufferOutAB() {
//	if(ptrProcessOutAB==bufferOut1_AB) {
//		ptrProcessOutAB = bufferOut2_AB;
//		ptrDmaOutAB = bufferOut1_AB;
//	} else {
//		ptrProcessOutAB = bufferOut1_AB;
//		ptrDmaOutAB = bufferOut2_AB;
//	}
//}
//
//void switchBufferOutCD() {
//	if(ptrProcessOutCD==bufferOut1_CD) {
//		ptrProcessOutCD = bufferOut2_CD;
//		ptrDmaOutCD = bufferOut1_CD;
//	} else {
//		ptrProcessOutCD = bufferOut1_CD;
//		ptrDmaOutCD = bufferOut2_CD;
//	}
//}

//#############################################################################
//Lectura y escritura de buffers
//#############################################################################
//size_t readData_I2S(uint8_t canal, q15_t *buff, size_t lenToRead) {
//	static uint8_t fIzq = 0, fDer = 0;
//	if(canal == IZQUIERDO)
//		fIzq = 1;
//
//	if(canal == DERECHO)
//		fDer = 1;
//
//	if(lenToRead > DATOS_P_CANAL)
//		return 0;
//
//	if(fDer&&fIzq) {
//		xSemaphoreTake(semDataReadyToProc,portMAX_DELAY);
//		fDer = 0;
//		fIzq = 0;
//	}
//	size_t i=0;
//	for(i=0; i<lenToRead;i++) {
//		buff[i] = (q15_t) ptrProcessIn[(2*i)+canal];
//	}
//
//	return i;
//}


//void writeData_I2S(uint8_t ampli, int16_t *datos, uint32_t lenToWrite,float gain) {
//	//Valido escritura
//	if(lenToWrite>DATOS_P_CANAL)
//		return;
//
//	//Escribo en las posiciones del vector de cada ampli
//
//
//	if(ampli==CHANNEL_0){
//		for(uint32_t i=0; i<lenToWrite; i++)
//			ptrProcessOutAB[i*2+IZQUIERDO] = (int16_t)datos[i]*gain;
//	}
//	if(ampli==CHANNEL_1){
//		for(uint32_t i=0; i<lenToWrite; i++)
//			ptrProcessOutAB[i*2+DERECHO] = (int16_t)datos[i]*gain;
//	}
//
//	if(ampli==CHANNEL_2){
//		for(uint32_t i=0; i<lenToWrite; i++)
//			ptrProcessOutCD[i*2+IZQUIERDO] = (int16_t)datos[i]*gain;
//	}
//
//	if(ampli==CHANNEL_3){
//		for(uint32_t i=0; i<lenToWrite; i++)
//			ptrProcessOutCD[i*2+DERECHO] = (int16_t)datos[i]*gain;
//	}
//
//}

//#############################################################################
//Recibir Transmitir Datos
//#############################################################################
//void recibirDatos() {
//	switchBufferIn();
//	//HAL_I2S_Receive_DMA(&hi2s2,(uint16_t *) ptrDmaIn, BUFF_SIZE);
//}
//
//void transmitirDatos() {
//	switchBufferOutAB();
//	switchBufferOutCD();
//
//	HAL_I2S_Transmit_DMA(&hi2s3,(uint16_t *) ptrDmaOutAB, BUFF_SIZE);
//
//	//Esta para datos de 8bits auqnue lo configure como trama de 16
//	//HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)ptrDmaOutCD, 2*BUFF_SIZE);
//}

//#############################################################################
//CALLBACKS DMA
//#############################################################################

//I2S 2
void callbackI2SRx_CMP() {
	static int i=0;
	ptrDmaIn = &bufferIn[PINPONG_BUFF_SIZE / 2];
	ptrProcessIn = &bufferIn[0];
	i++;
//	if(i > 20){
//		i = 0;
//		HAL_I2S_DMAPause(&hi2s2);
//		HAL_I2S_DMAPause(&hi2s3);
//		while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_RESET);    //use GPIO_PIN_SET for the other I2S mode
//		while (HAL_GPIO_ReadPin(WS_PIN_GPIO_Port, WS_PIN_Pin) != GPIO_PIN_SET);    //use GPIO_PIN_RESET for the other I2S mode   <<<<<<<< THIS
//		//HAL_I2S_Receive_DMA(&hi2s2,(uint16_t *) ptrDmaIn, PINPONG_BUFF_SIZE);
//		HAL_I2S_DMAResume(&hi2s2);
//		HAL_I2S_DMAResume(&hi2s3);
//
//	}
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void callbackI2SRx_HALF() {
	ptrDmaIn = &bufferIn[0];
	ptrProcessIn = &bufferIn[PINPONG_BUFF_SIZE / 2];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semRx_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

//I2S 3
void callbackI2STx_CMP() {
	ptrDmaOutAB = &bufferOut_AB[PINPONG_BUFF_SIZE / 2];
	ptrProcessOutAB = &bufferOut_AB[0];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
void callbackI2STx_HALF() {
	ptrDmaOutAB = &bufferOut_AB[0];
	ptrProcessOutAB = &bufferOut_AB[PINPONG_BUFF_SIZE / 2];

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( semTxAB_I2S, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


//#############################################################################
//Task recieve
//#############################################################################
void task_I2S_recieve() {
	while(1) {
		//Proceso

		//xSemaphoreTake(semDataReadyToSendDMA,portMAX_DELAY);

		//Transmito
		xSemaphoreTake(semTxAB_I2S,portMAX_DELAY);
		//xSemaphoreTake(semTxCD_I2S,portMAX_DELAY);

		//Recibo
		xSemaphoreTake(semRx_I2S,portMAX_DELAY);

		//Cuando cumplí con las 3 al mismo tiempo puedo volver a repetir el proceso
		//En las interrupciones:
			//give(semRx_I2S)
			//give(semTxAB_I2S)
			//give(semTxCD_I2S)
		//En funcion de procesamiento
			//give(semProc_I2S) con lberarSemaforoProc()
		//xSemaphoreGive(semDataReadyToProc);

		//recibirDatos();

		//transmitirDatos();

		//Aviso que ya podés volver a leer
		//Procesamiento de prueba en esta misma tarea

	pruebaLoopback();
	}
}

//Cuando terminas de cargar buffers de salida llamas a esta función
void lberarSemaforoProc() {
	xSemaphoreGive(semDataReadyToSendDMA);
}

//#############################################################################
// Ejemplo de procesamiento (no se usa en ninun lado esta funcion)
//#############################################################################
q15_t dataIzq[DATOS_P_CANAL];
q15_t dataDer[DATOS_P_CANAL];

void pruebaLoopback() {
	//Leo
	for(int i =0;i<BUFF_SIZE;i++){
		ptrProcessOutAB[i] = ptrProcessIn[i];
	}

//	readData_I2S(IZQUIERDO, dataIzq, DATOS_P_CANAL);
//	readData_I2S(DERECHO,   dataDer, DATOS_P_CANAL);
//
//	//Escribo
//	writeData_I2S(CHANNEL_0, dataIzq, DATOS_P_CANAL,1);
//	writeData_I2S(CHANNEL_1, dataIzq, DATOS_P_CANAL,1);
//
//	writeData_I2S(CHANNEL_2, dataDer, DATOS_P_CANAL,1);
//	writeData_I2S(CHANNEL_3, dataDer, DATOS_P_CANAL,1);
//
//	//Libero semaforo de procesamiento
//	lberarSemaforoProc();
}
