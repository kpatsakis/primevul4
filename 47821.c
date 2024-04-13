int BN_GF2m_poly2arr(const BIGNUM *a, int p[], int max)
{
    int i, j, k = 0;
    BN_ULONG mask;

    if (BN_is_zero(a))
        return 0;

    for (i = a->top - 1; i >= 0; i--) {
        if (!a->d[i])
            /* skip word if a->d[i] == 0 */
            continue;
        mask = BN_TBIT;
        for (j = BN_BITS2 - 1; j >= 0; j--) {
            if (a->d[i] & mask) {
                if (k < max)
                    p[k] = BN_BITS2 * i + j;
                k++;
            }
            mask >>= 1;
        }
    }

    if (k < max) {
        p[k] = -1;
        k++;
    }

    return k;
}
