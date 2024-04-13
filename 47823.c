static void bn_GF2m_mul_2x2(BN_ULONG *r, const BN_ULONG a1, const BN_ULONG a0,
                            const BN_ULONG b1, const BN_ULONG b0)
{
    BN_ULONG m1, m0;
    /* r[3] = h1, r[2] = h0; r[1] = l1; r[0] = l0 */
    bn_GF2m_mul_1x1(r + 3, r + 2, a1, b1);
    bn_GF2m_mul_1x1(r + 1, r, a0, b0);
    bn_GF2m_mul_1x1(&m1, &m0, a0 ^ a1, b0 ^ b1);
    /* Correction on m1 ^= l1 ^ h1; m0 ^= l0 ^ h0; */
    r[2] ^= m1 ^ r[1] ^ r[3];   /* h0 ^= m1 ^ l1 ^ h1; */
    r[1] = r[3] ^ r[2] ^ r[0] ^ m1 ^ m0; /* l1 ^= l0 ^ h0 ^ m0; */
}
