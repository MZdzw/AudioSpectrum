#ifndef __FFT_H__
#define __FFT_H__
#include "adc.h"

#define FFT_SAMPLES 512

typedef int Spectrum;

typedef enum Spectrum_State_e
{
    SAMPLING,
    SAMPLING_DONE,
    CALCULATED_FFT
} Spectrum_State_e;

typedef enum Sound_Level_e
{
    NO_SOUND,
    LOW_SOUND,
    NORMAL_SOUND
} Sound_Level_e;

typedef struct FFT_Sound_Properties_t
{
    Spectrum maxVal;
    uint16_t index;
    Sound_Level_e sound;
} FFT_Sound_Properties_t;

void InitFFT(void);
void CalulateFFT(void);
Spectrum* GetFrequencies(void);
void StartConversion(void);
Spectrum_State_e GetFFTState(void);
FFT_Sound_Properties_t CheckSound(void);

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

#endif