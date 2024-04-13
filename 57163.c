static int vp8_read_mv_component(VP56RangeCoder *c, const uint8_t *p)
{
    return read_mv_component(c, p, 0);
}
