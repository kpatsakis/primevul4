static void cdxl_decode_ham6(CDXLVideoContext *c, AVFrame *frame)
{
    AVCodecContext *avctx = c->avctx;
    uint32_t new_palette[16], r, g, b;
    uint8_t *ptr, *out, index, op;
    int x, y;

    ptr = c->new_video;
    out = frame->data[0];

    import_palette(c, new_palette);
    import_format(c, avctx->width, c->new_video);

    for (y = 0; y < avctx->height; y++) {
        r = new_palette[0] & 0xFF0000;
        g = new_palette[0] & 0xFF00;
        b = new_palette[0] & 0xFF;
        for (x = 0; x < avctx->width; x++) {
            index  = *ptr++;
            op     = index >> 4;
            index &= 15;
            switch (op) {
            case 0:
                r = new_palette[index] & 0xFF0000;
                g = new_palette[index] & 0xFF00;
                b = new_palette[index] & 0xFF;
                break;
            case 1:
                b = index * 0x11;
                break;
            case 2:
                r = index * 0x11 << 16;
                break;
            case 3:
                g = index * 0x11 << 8;
                break;
            }
            AV_WL24(out + x * 3, r | g | b);
        }
        out += frame->linesize[0];
    }
}
