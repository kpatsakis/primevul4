void SlopeLimiting(cmsToneCurve* g)
{
    int BeginVal, EndVal;
    int AtBegin = (int) floor((cmsFloat64Number) g ->nEntries * 0.02 + 0.5);   // Cutoff at 2%
    int AtEnd   = g ->nEntries - AtBegin - 1;                                  // And 98%
    cmsFloat64Number Val, Slope, beta;
    int i;

    if (cmsIsToneCurveDescending(g)) {
        BeginVal = 0xffff; EndVal = 0;
    }
    else {
        BeginVal = 0; EndVal = 0xffff;
    }

    Val   = g ->Table16[AtBegin];
    Slope = (Val - BeginVal) / AtBegin;
    beta  = Val - Slope * AtBegin;

    for (i=0; i < AtBegin; i++)
        g ->Table16[i] = _cmsQuickSaturateWord(i * Slope + beta);

    Val   = g ->Table16[AtEnd];
    Slope = (EndVal - Val) / AtBegin;   // AtBegin holds the X interval, which is same in both cases
    beta  = Val - Slope * AtEnd;

    for (i = AtEnd; i < (int) g ->nEntries; i++)
        g ->Table16[i] = _cmsQuickSaturateWord(i * Slope + beta);
}
