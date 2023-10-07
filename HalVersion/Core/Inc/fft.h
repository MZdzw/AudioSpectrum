#ifndef __FFT_H__
#define __FFT_H__
#include "adc.h"

#define FFT_SAMPLES 512

typedef enum Spectrum_State_e
{
    SAMPLING,
    SAMPLING_DONE,
    CALCULATED_FFT
} Spectrum_State_e;

typedef int Spectrum;

void InitFFT(void);
void CalulateFFT(void);
Spectrum* GetFrequencies(void);
void StartConversion(void);
Spectrum_State_e GetFFTState(void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#endif