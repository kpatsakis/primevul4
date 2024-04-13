void ff_color_frame(AVFrame *frame, const int c[4])
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(frame->format);
    int p, y, x;

    av_assert0(desc->flags & AV_PIX_FMT_FLAG_PLANAR);

    for (p = 0; p<desc->nb_components; p++) {
        uint8_t *dst = frame->data[p];
        int is_chroma = p == 1 || p == 2;
        int bytes  = is_chroma ? AV_CEIL_RSHIFT(frame->width,  desc->log2_chroma_w) : frame->width;
        int height = is_chroma ? AV_CEIL_RSHIFT(frame->height, desc->log2_chroma_h) : frame->height;
        for (y = 0; y < height; y++) {
            if (desc->comp[0].depth >= 9) {
                for (x = 0; x<bytes; x++)
                    ((uint16_t*)dst)[x] = c[p];
            }else
                memset(dst, c[p], bytes);
            dst += frame->linesize[p];
        }
    }
}
