static void import_palette(CDXLVideoContext *c, uint32_t *new_palette)
{
    int i;

    for (i = 0; i < c->palette_size / 2; i++) {
        unsigned rgb = AV_RB16(&c->palette[i * 2]);
        unsigned r   = ((rgb >> 8) & 0xF) * 0x11;
        unsigned g   = ((rgb >> 4) & 0xF) * 0x11;
        unsigned b   =  (rgb       & 0xF) * 0x11;
        AV_WN32(&new_palette[i], (0xFFU << 24) | (r << 16) | (g << 8) | b);
    }
}
