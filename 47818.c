int BN_GF2m_mod_sqr_arr(BIGNUM *r, const BIGNUM *a, const int p[],
                        BN_CTX *ctx)
{
    int i, ret = 0;
    BIGNUM *s;

    bn_check_top(a);
    BN_CTX_start(ctx);
    if ((s = BN_CTX_get(ctx)) == NULL)
        return 0;
    if (!bn_wexpand(s, 2 * a->top))
        goto err;

    for (i = a->top - 1; i >= 0; i--) {
        s->d[2 * i + 1] = SQR1(a->d[i]);
        s->d[2 * i] = SQR0(a->d[i]);
    }

    s->top = 2 * a->top;
    bn_correct_top(s);
    if (!BN_GF2m_mod_arr(r, s, p))
        goto err;
    bn_check_top(r);
    ret = 1;
 err:
    BN_CTX_end(ctx);
    return ret;
}
