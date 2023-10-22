#include "adc.h"
#include "stm32f401xc.h"    // must be included before "arm_math.h" because of "__FPU_PRESENT" define
#include "arm_math.h"
#include "fft.h"
#include "tim.h"

static arm_rfft_fast_instance_f32 fft_handler;
static uint16_t musicADC[FFT_SAMPLES];
static float fft_in_buf[FFT_SAMPLES];
static float fft_out_buf[FFT_SAMPLES];
static Spectrum freqs[FFT_SAMPLES >> 1];
static Spectrum_State_e state;
static Spectrum analyzedFreqs[7];

static float complexABS(float real, float compl)
{
    return sqrtf(real * real + compl * compl);
}

void InitFFT(void)
{
    arm_rfft_fast_init_f32(&fft_handler, FFT_SAMPLES);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)musicADC, FFT_SAMPLES); // You have to start ADC with DMA
    state = SAMPLING;
}

void CalulateFFT(void)
{
    for(int i = 0; i < 512; i++)
    {
        fft_in_buf[i] = ((float)musicADC[i] / 255) * 3.3f;
    }
    arm_rfft_fast_f32(&fft_handler, fft_in_buf, fft_out_buf, 0);
    state = CALCULATED_FFT;

    // int freqs[FFT_SAMPLES >> 1];
    int freqpoint = 0;

    //calculate abs values and linear-to-dB
    for (int i = 0; i < FFT_SAMPLES; i += 2)
    {
            freqs[freqpoint] = (int)(complexABS(fft_out_buf[i], fft_out_buf[i+1]));
            if (freqs[freqpoint] < 0) 
                freqs[freqpoint] = 0;
            freqpoint++;
    }
    //sampling frequency ~ 44,094 KHZ
    //formula: freqpoint = (freq / (sampling_frequency / 2)) * (FFT_SAMPLES / 2)
    //  |   |
    //  V   V
    // freq = freqpoint * sampling_frequency / FFT_SAMPLES

    // freqs[1];   ->   86 Hz
    // freqs[3];   ->   258 Hz
    // freqs[6];   ->   517 Hz
    // freqs[12];  ->   1033 Hz
    // freqs[23];  ->   1981 Hz
    // freqs[46];  ->   3962 Hz
    // freqs[70];  ->   6028 Hz
    // freqs[93];  ->   8009 Hz
}

Spectrum* GetFrequencies(void)
{
    return freqs;
}

void StartConversion(void)
{
    state = SAMPLING;
    __disable_irq();
    // start another conversion
    HAL_TIM_Base_Start(&htim3);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)musicADC, 512);
    __enable_irq();
}

Spectrum_State_e GetFFTState(void)
{
    return state;
}

FFT_Sound_Properties_t CheckSound(void)
{
    FFT_Sound_Properties_t fftSoundProp = {0, 1, NORMAL_SOUND};
    analyzedFreqs[1] = freqs[1];
    analyzedFreqs[2] = freqs[3];
    analyzedFreqs[3] = freqs[6];
    analyzedFreqs[4] = freqs[12];
    analyzedFreqs[5] = freqs[23];
    analyzedFreqs[6] = freqs[46];

    uint8_t underTresholdCounter = 0;
    uint8_t minRanges = 0;

    for (uint8_t i = 1; i <= 6; i++)
    {
        if (analyzedFreqs[i] < 10)
        {
                underTresholdCounter++;
        }
        else if(analyzedFreqs[i] >= 10 && analyzedFreqs[i] < 30)
        {
                underTresholdCounter++;
                minRanges++;
        }

        if (analyzedFreqs[i] >= fftSoundProp.maxVal)
        {
                fftSoundProp.maxVal = analyzedFreqs[i];
                fftSoundProp.index = i;
        }

    }
    if (underTresholdCounter == 6)
    {
        fftSoundProp.sound = (minRanges >= 3) ? LOW_SOUND : NO_SOUND;
    }

    return fftSoundProp;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    (void)hadc;
    HAL_TIM_Base_Stop(&htim3);
    HAL_ADC_Stop_DMA(&hadc1);
    state = SAMPLING_DONE;
}