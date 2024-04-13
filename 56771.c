static av_always_inline int filter_fast_3320(APEPredictor *p,
                                             const int decoded, const int filter,
                                             const int delayA)
{
    int32_t predictionA;

    p->buf[delayA] = p->lastA[filter];
    if (p->sample_pos < 3) {
        p->lastA[filter]   = decoded;
        p->filterA[filter] = decoded;
        return decoded;
    }

    predictionA = p->buf[delayA] * 2 - p->buf[delayA - 1];
    p->lastA[filter] = decoded + (predictionA  * p->coeffsA[filter][0] >> 9);

    if ((decoded ^ predictionA) > 0)
        p->coeffsA[filter][0]++;
    else
        p->coeffsA[filter][0]--;

    p->filterA[filter] += p->lastA[filter];

    return p->filterA[filter];
}
