#include "dds1508.h"

double_t freqDiscret = 800;
double_t discret = 24;

void DDS1508_SetDiscretisationFreq(double_t sfreqDiscret)
{
    freqDiscret = sfreqDiscret;
}

uint64_t DDS1508_CalcFreqWord(double_t freqTarget)
{
    return (freqTarget/freqDiscret) * pow(2, 48);
}

uint64_t DDS1508_CalcDFWord(double_t freqStart, double_t freqStop, double_t lfmLength)
{
    return (pow(2, 48)/freqDiscret) * (freqStop - freqStop)/(freqDiscret * lfmLength/discret/4);
}

uint64_t DDS1508_CalcTWord(double_t lentgh)
{
    return freqDiscret * lentgh/discret/4;
}
