int BN_GF2m_mod_solve_quad_arr(BIGNUM *r, const BIGNUM *a_, const int p[],
                               BN_CTX *ctx)
{
    int ret = 0, count = 0, j;
    BIGNUM *a, *z, *rho, *w, *w2, *tmp;

    bn_check_top(a_);

    if (!p[0]) {
        /* reduction mod 1 => return 0 */
        BN_zero(r);
        return 1;
    }

    BN_CTX_start(ctx);
    a = BN_CTX_get(ctx);
    z = BN_CTX_get(ctx);
    w = BN_CTX_get(ctx);
    if (w == NULL)
        goto err;

    if (!BN_GF2m_mod_arr(a, a_, p))
        goto err;

    if (BN_is_zero(a)) {
        BN_zero(r);
        ret = 1;
        goto err;
    }

    if (p[0] & 0x1) {           /* m is odd */
        /* compute half-trace of a */
        if (!BN_copy(z, a))
            goto err;
        for (j = 1; j <= (p[0] - 1) / 2; j++) {
            if (!BN_GF2m_mod_sqr_arr(z, z, p, ctx))
                goto err;
            if (!BN_GF2m_mod_sqr_arr(z, z, p, ctx))
                goto err;
            if (!BN_GF2m_add(z, z, a))
                goto err;
        }

    } else {                    /* m is even */

        rho = BN_CTX_get(ctx);
        w2 = BN_CTX_get(ctx);
        tmp = BN_CTX_get(ctx);
        if (tmp == NULL)
            goto err;
        do {
            if (!BN_rand(rho, p[0], 0, 0))
                goto err;
            if (!BN_GF2m_mod_arr(rho, rho, p))
                goto err;
            BN_zero(z);
            if (!BN_copy(w, rho))
                goto err;
            for (j = 1; j <= p[0] - 1; j++) {
                if (!BN_GF2m_mod_sqr_arr(z, z, p, ctx))
                    goto err;
                if (!BN_GF2m_mod_sqr_arr(w2, w, p, ctx))
                    goto err;
                if (!BN_GF2m_mod_mul_arr(tmp, w2, a, p, ctx))
                    goto err;
                if (!BN_GF2m_add(z, z, tmp))
                    goto err;
                if (!BN_GF2m_add(w, w2, rho))
                    goto err;
            }
            count++;
        } while (BN_is_zero(w) && (count < MAX_ITERATIONS));
        if (BN_is_zero(w)) {
            BNerr(BN_F_BN_GF2M_MOD_SOLVE_QUAD_ARR, BN_R_TOO_MANY_ITERATIONS);
            goto err;
        }
    }

    if (!BN_GF2m_mod_sqr_arr(w, z, p, ctx))
        goto err;
    if (!BN_GF2m_add(w, z, w))
        goto err;
    if (BN_GF2m_cmp(w, a)) {
        BNerr(BN_F_BN_GF2M_MOD_SOLVE_QUAD_ARR, BN_R_NO_SOLUTION);
        goto err;
    }

    if (!BN_copy(r, z))
        goto err;
    bn_check_top(r);

    ret = 1;

 err:
    BN_CTX_end(ctx);
    return ret;
}
