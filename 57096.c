static void init_rangecoder(RangeCoder *rc, GetByteContext *gb)
{
    rc->code1 = 0;
    rc->range = 0xFFFFFFFFU;
    rc->code  = bytestream2_get_be32(gb);
}
