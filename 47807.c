int BN_GF2m_arr2poly(const int p[], BIGNUM *a)
{
    int i;

    bn_check_top(a);
    BN_zero(a);
    for (i = 0; p[i] != -1; i++) {
        if (BN_set_bit(a, p[i]) == 0)
            return 0;
    }
    bn_check_top(a);

    return 1;
}
