int BN_GF2m_mod_arr(BIGNUM *r, const BIGNUM *a, const int p[])
{
    int j, k;
    int n, dN, d0, d1;
    BN_ULONG zz, *z;

    bn_check_top(a);

    if (!p[0]) {
        /* reduction mod 1 => return 0 */
        BN_zero(r);
        return 1;
    }

    /*
     * Since the algorithm does reduction in the r value, if a != r, copy the
     * contents of a into r so we can do reduction in r.
     */
    if (a != r) {
        if (!bn_wexpand(r, a->top))
            return 0;
        for (j = 0; j < a->top; j++) {
            r->d[j] = a->d[j];
        }
        r->top = a->top;
    }
    z = r->d;

    /* start reduction */
    dN = p[0] / BN_BITS2;
    for (j = r->top - 1; j > dN;) {
        zz = z[j];
        if (z[j] == 0) {
            j--;
            continue;
        }
        z[j] = 0;

        for (k = 1; p[k] != 0; k++) {
            /* reducing component t^p[k] */
            n = p[0] - p[k];
            d0 = n % BN_BITS2;
            d1 = BN_BITS2 - d0;
            n /= BN_BITS2;
            z[j - n] ^= (zz >> d0);
            if (d0)
                z[j - n - 1] ^= (zz << d1);
        }

        /* reducing component t^0 */
        n = dN;
        d0 = p[0] % BN_BITS2;
        d1 = BN_BITS2 - d0;
        z[j - n] ^= (zz >> d0);
        if (d0)
            z[j - n - 1] ^= (zz << d1);
    }

    /* final round of reduction */
    while (j == dN) {

        d0 = p[0] % BN_BITS2;
        zz = z[dN] >> d0;
        if (zz == 0)
            break;
        d1 = BN_BITS2 - d0;

        /* clear up the top d1 bits */
        if (d0)
            z[dN] = (z[dN] << d1) >> d1;
        else
            z[dN] = 0;
        z[0] ^= zz;             /* reduction t^0 component */

        for (k = 1; p[k] != 0; k++) {
            BN_ULONG tmp_ulong;

            /* reducing component t^p[k] */
            n = p[k] / BN_BITS2;
            d0 = p[k] % BN_BITS2;
            d1 = BN_BITS2 - d0;
            z[n] ^= (zz << d0);
            if (d0 && (tmp_ulong = zz >> d1))
                z[n + 1] ^= tmp_ulong;
        }

    }

    bn_correct_top(r);
    return 1;
}
