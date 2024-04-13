static void long_filter_ehigh_3830(int32_t *buffer, int length)
{
    int i, j;
    int32_t dotprod, sign;
    int32_t coeffs[8] = { 0 }, delay[8] = { 0 };

    for (i = 0; i < length; i++) {
        dotprod = 0;
        sign = APESIGN(buffer[i]);
        for (j = 7; j >= 0; j--) {
            dotprod += delay[j] * coeffs[j];
            coeffs[j] += ((delay[j] >> 31) | 1) * sign;
        }
        for (j = 7; j > 0; j--)
            delay[j] = delay[j - 1];
        delay[0] = buffer[i];
        buffer[i] -= dotprod >> 9;
    }
}
