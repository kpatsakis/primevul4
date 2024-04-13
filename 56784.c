static av_always_inline int predictor_update_filter(APEPredictor *p,
                                                    const int decoded, const int filter,
                                                    const int delayA,  const int delayB,
                                                    const int adaptA,  const int adaptB)
{
    int32_t predictionA, predictionB, sign;

    p->buf[delayA]     = p->lastA[filter];
    p->buf[adaptA]     = APESIGN(p->buf[delayA]);
    p->buf[delayA - 1] = p->buf[delayA] - p->buf[delayA - 1];
    p->buf[adaptA - 1] = APESIGN(p->buf[delayA - 1]);

    predictionA = p->buf[delayA    ] * p->coeffsA[filter][0] +
                  p->buf[delayA - 1] * p->coeffsA[filter][1] +
                  p->buf[delayA - 2] * p->coeffsA[filter][2] +
                  p->buf[delayA - 3] * p->coeffsA[filter][3];

    /*  Apply a scaled first-order filter compression */
    p->buf[delayB]     = p->filterA[filter ^ 1] - ((p->filterB[filter] * 31) >> 5);
    p->buf[adaptB]     = APESIGN(p->buf[delayB]);
    p->buf[delayB - 1] = p->buf[delayB] - p->buf[delayB - 1];
    p->buf[adaptB - 1] = APESIGN(p->buf[delayB - 1]);
    p->filterB[filter] = p->filterA[filter ^ 1];

    predictionB = p->buf[delayB    ] * p->coeffsB[filter][0] +
                  p->buf[delayB - 1] * p->coeffsB[filter][1] +
                  p->buf[delayB - 2] * p->coeffsB[filter][2] +
                  p->buf[delayB - 3] * p->coeffsB[filter][3] +
                  p->buf[delayB - 4] * p->coeffsB[filter][4];

    p->lastA[filter] = decoded + ((predictionA + (predictionB >> 1)) >> 10);
    p->filterA[filter] = p->lastA[filter] + ((p->filterA[filter] * 31) >> 5);

    sign = APESIGN(decoded);
    p->coeffsA[filter][0] += p->buf[adaptA    ] * sign;
    p->coeffsA[filter][1] += p->buf[adaptA - 1] * sign;
    p->coeffsA[filter][2] += p->buf[adaptA - 2] * sign;
    p->coeffsA[filter][3] += p->buf[adaptA - 3] * sign;
    p->coeffsB[filter][0] += p->buf[adaptB    ] * sign;
    p->coeffsB[filter][1] += p->buf[adaptB - 1] * sign;
    p->coeffsB[filter][2] += p->buf[adaptB - 2] * sign;
    p->coeffsB[filter][3] += p->buf[adaptB - 3] * sign;
    p->coeffsB[filter][4] += p->buf[adaptB - 4] * sign;

    return p->filterA[filter];
}
