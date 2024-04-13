int BN_GF2m_mod_sqrt_arr(BIGNUM *r, const BIGNUM *a, const int p[],
                         BN_CTX *ctx)
{
    int ret = 0;
    BIGNUM *u;

    bn_check_top(a);

    if (!p[0]) {
        /* reduction mod 1 => return 0 */
        BN_zero(r);
        return 1;
    }

    BN_CTX_start(ctx);
    if ((u = BN_CTX_get(ctx)) == NULL)
        goto err;

    if (!BN_set_bit(u, p[0] - 1))
        goto err;
    ret = BN_GF2m_mod_exp_arr(r, a, u, p, ctx);
    bn_check_top(r);

 err:
    BN_CTX_end(ctx);
    return ret;
}
