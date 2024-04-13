static av_always_inline uint8_t clamp_add_subtract_half(int a, int b, int c)
{
    int d = a + b >> 1;
    return av_clip_uint8(d + (d - c) / 2);
}
