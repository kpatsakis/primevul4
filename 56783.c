static av_always_inline int predictor_update_3930(APEPredictor *p,
                                                  const int decoded, const int filter,
                                                  const int delayA)
{
    int32_t predictionA, sign;
    int32_t d0, d1, d2, d3;

    p->buf[delayA]     = p->lastA[filter];
    d0 = p->buf[delayA    ];
    d1 = p->buf[delayA    ] - p->buf[delayA - 1];
    d2 = p->buf[delayA - 1] - p->buf[delayA - 2];
    d3 = p->buf[delayA - 2] - p->buf[delayA - 3];

    predictionA = d0 * p->coeffsA[filter][0] +
                  d1 * p->coeffsA[filter][1] +
                  d2 * p->coeffsA[filter][2] +
                  d3 * p->coeffsA[filter][3];

    p->lastA[filter] = decoded + (predictionA >> 9);
    p->filterA[filter] = p->lastA[filter] + ((p->filterA[filter] * 31) >> 5);

    sign = APESIGN(decoded);
    p->coeffsA[filter][0] += ((d0 < 0) * 2 - 1) * sign;
    p->coeffsA[filter][1] += ((d1 < 0) * 2 - 1) * sign;
    p->coeffsA[filter][2] += ((d2 < 0) * 2 - 1) * sign;
    p->coeffsA[filter][3] += ((d3 < 0) * 2 - 1) * sign;

    return p->filterA[filter];
}
