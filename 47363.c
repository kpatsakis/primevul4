void ff_h264_draw_horiz_band(H264Context *h, int y, int height)
{
    AVCodecContext *avctx = h->avctx;
    AVFrame *cur  = &h->cur_pic.f;
    AVFrame *last = h->ref_list[0][0].f.data[0] ? &h->ref_list[0][0].f : NULL;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(avctx->pix_fmt);
    int vshift = desc->log2_chroma_h;
    const int field_pic = h->picture_structure != PICT_FRAME;
    if (field_pic) {
        height <<= 1;
        y      <<= 1;
    }

    height = FFMIN(height, avctx->height - y);

    if (field_pic && h->first_field && !(avctx->slice_flags & SLICE_FLAG_ALLOW_FIELD))
        return;

    if (avctx->draw_horiz_band) {
        AVFrame *src;
        int offset[AV_NUM_DATA_POINTERS];
        int i;

        if (cur->pict_type == AV_PICTURE_TYPE_B || h->low_delay ||
            (avctx->slice_flags & SLICE_FLAG_CODED_ORDER))
            src = cur;
        else if (last)
            src = last;
        else
            return;

        offset[0] = y * src->linesize[0];
        offset[1] =
        offset[2] = (y >> vshift) * src->linesize[1];
        for (i = 3; i < AV_NUM_DATA_POINTERS; i++)
            offset[i] = 0;

        emms_c();

        avctx->draw_horiz_band(avctx, src, offset,
                               y, h->picture_structure, height);
    }
}
