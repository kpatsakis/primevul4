static void bn_GF2m_mul_1x1(BN_ULONG *r1, BN_ULONG *r0, const BN_ULONG a,
                            const BN_ULONG b)
{
    register BN_ULONG h, l, s;
    BN_ULONG tab[16], top3b = a >> 61;
    register BN_ULONG a1, a2, a4, a8;

    a1 = a & (0x1FFFFFFFFFFFFFFFULL);
    a2 = a1 << 1;
    a4 = a2 << 1;
    a8 = a4 << 1;

    tab[0] = 0;
    tab[1] = a1;
    tab[2] = a2;
    tab[3] = a1 ^ a2;
    tab[4] = a4;
    tab[5] = a1 ^ a4;
    tab[6] = a2 ^ a4;
    tab[7] = a1 ^ a2 ^ a4;
    tab[8] = a8;
    tab[9] = a1 ^ a8;
    tab[10] = a2 ^ a8;
    tab[11] = a1 ^ a2 ^ a8;
    tab[12] = a4 ^ a8;
    tab[13] = a1 ^ a4 ^ a8;
    tab[14] = a2 ^ a4 ^ a8;
    tab[15] = a1 ^ a2 ^ a4 ^ a8;

    s = tab[b & 0xF];
    l = s;
    s = tab[b >> 4 & 0xF];
    l ^= s << 4;
    h = s >> 60;
    s = tab[b >> 8 & 0xF];
    l ^= s << 8;
    h ^= s >> 56;
    s = tab[b >> 12 & 0xF];
    l ^= s << 12;
    h ^= s >> 52;
    s = tab[b >> 16 & 0xF];
    l ^= s << 16;
    h ^= s >> 48;
    s = tab[b >> 20 & 0xF];
    l ^= s << 20;
    h ^= s >> 44;
    s = tab[b >> 24 & 0xF];
    l ^= s << 24;
    h ^= s >> 40;
    s = tab[b >> 28 & 0xF];
    l ^= s << 28;
    h ^= s >> 36;
    s = tab[b >> 32 & 0xF];
    l ^= s << 32;
    h ^= s >> 32;
    s = tab[b >> 36 & 0xF];
    l ^= s << 36;
    h ^= s >> 28;
    s = tab[b >> 40 & 0xF];
    l ^= s << 40;
    h ^= s >> 24;
    s = tab[b >> 44 & 0xF];
    l ^= s << 44;
    h ^= s >> 20;
    s = tab[b >> 48 & 0xF];
    l ^= s << 48;
    h ^= s >> 16;
    s = tab[b >> 52 & 0xF];
    l ^= s << 52;
    h ^= s >> 12;
    s = tab[b >> 56 & 0xF];
    l ^= s << 56;
    h ^= s >> 8;
    s = tab[b >> 60];
    l ^= s << 60;
    h ^= s >> 4;

    /* compensate for the top three bits of a */

    if (top3b & 01) {
        l ^= b << 61;
        h ^= b >> 3;
    }
    if (top3b & 02) {
        l ^= b << 62;
        h ^= b >> 2;
    }
    if (top3b & 04) {
        l ^= b << 63;
        h ^= b >> 1;
    }

    *r1 = h;
    *r0 = l;
}
