static int decode_blck(GetByteContext *gb, uint8_t *frame, int width, int height)
{
    memset(frame, 0, width * height);
    return 0;
}
