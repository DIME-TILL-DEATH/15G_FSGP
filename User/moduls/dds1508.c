#include "dds1508.h"

#include "debug.h"

double_t freqDiscret = 800;
double_t discret = 24;

void DDS1508_SetDiscretisationFreq(double_t sfreqDiscret)
{
    freqDiscret = sfreqDiscret;
}

int64_t DDS1508_CalcFreqWord(double_t freqTarget)
{
    return (freqTarget/freqDiscret) * pow(2, 48);
}

int64_t DDS1508_CalcDFWord(double_t freqStart, double_t freqStop, double_t lfmLength)
{
    int64_t freqWord = DDS1508_CalcFreqWord(freqStop - freqStart);
    int64_t timeWord =  DDS1508_CalcTWord(lfmLength);

    int64_t dF = freqWord/timeWord;
//    int64_t dFRounded = round(dF);

    return dF;
}

uint64_t DDS1508_CalcTWord(double_t lentgh)
{
    return freqDiscret * lentgh/discret/4;
}
