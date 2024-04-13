int BN_GF2m_mod_div_arr(BIGNUM *r, const BIGNUM *yy, const BIGNUM *xx,
                        const int p[], BN_CTX *ctx)
{
    BIGNUM *field;
    int ret = 0;

    bn_check_top(yy);
    bn_check_top(xx);

    BN_CTX_start(ctx);
    if ((field = BN_CTX_get(ctx)) == NULL)
        goto err;
    if (!BN_GF2m_arr2poly(p, field))
        goto err;

    ret = BN_GF2m_mod_div(r, yy, xx, field, ctx);
    bn_check_top(r);

 err:
    BN_CTX_end(ctx);
    return ret;
}
