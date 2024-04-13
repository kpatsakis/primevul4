int BN_GF2m_mod_mul_arr(BIGNUM *r, const BIGNUM *a, const BIGNUM *b,
                        const int p[], BN_CTX *ctx)
{
    int zlen, i, j, k, ret = 0;
    BIGNUM *s;
    BN_ULONG x1, x0, y1, y0, zz[4];

    bn_check_top(a);
    bn_check_top(b);

    if (a == b) {
        return BN_GF2m_mod_sqr_arr(r, a, p, ctx);
    }

    BN_CTX_start(ctx);
    if ((s = BN_CTX_get(ctx)) == NULL)
        goto err;

    zlen = a->top + b->top + 4;
    if (!bn_wexpand(s, zlen))
        goto err;
    s->top = zlen;

    for (i = 0; i < zlen; i++)
        s->d[i] = 0;

    for (j = 0; j < b->top; j += 2) {
        y0 = b->d[j];
        y1 = ((j + 1) == b->top) ? 0 : b->d[j + 1];
        for (i = 0; i < a->top; i += 2) {
            x0 = a->d[i];
            x1 = ((i + 1) == a->top) ? 0 : a->d[i + 1];
            bn_GF2m_mul_2x2(zz, x1, x0, y1, y0);
            for (k = 0; k < 4; k++)
                s->d[i + j + k] ^= zz[k];
        }
    }

    bn_correct_top(s);
    if (BN_GF2m_mod_arr(r, s, p))
        ret = 1;
    bn_check_top(r);

 err:
    BN_CTX_end(ctx);
    return ret;
}
