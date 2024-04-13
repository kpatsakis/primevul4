pkinit_check_dh_params(BIGNUM * p1, BIGNUM * p2, BIGNUM * g1, BIGNUM * q1)
{
    BIGNUM *g2 = NULL, *q2 = NULL;
    int retval = -1;

    if (!BN_cmp(p1, p2)) {
        g2 = BN_new();
        BN_set_word(g2, DH_GENERATOR_2);
        if (!BN_cmp(g1, g2)) {
            q2 = BN_new();
            BN_rshift1(q2, p1);
            if (!BN_cmp(q1, q2)) {
                pkiDebug("good %d dhparams\n", BN_num_bits(p1));
                retval = 0;
            } else
                pkiDebug("bad group 2 q dhparameter\n");
            BN_free(q2);
        } else
            pkiDebug("bad g dhparameter\n");
        BN_free(g2);
    } else
        pkiDebug("p is not well-known group 2 dhparameter\n");

    return retval;
}
