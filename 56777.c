static void long_filter_high_3800(int32_t *buffer, int order, int shift, int length)
{
    int i, j;
    int32_t dotprod, sign;
    int32_t coeffs[256], delay[256];

    if (order >= length)
        return;

    memset(coeffs, 0, order * sizeof(*coeffs));
    for (i = 0; i < order; i++)
        delay[i] = buffer[i];
    for (i = order; i < length; i++) {
        dotprod = 0;
        sign = APESIGN(buffer[i]);
        for (j = 0; j < order; j++) {
            dotprod += delay[j] * coeffs[j];
            coeffs[j] += ((delay[j] >> 31) | 1) * sign;
        }
        buffer[i] -= dotprod >> shift;
        for (j = 0; j < order - 1; j++)
            delay[j] = delay[j + 1];
        delay[order - 1] = buffer[i];
    }
}
