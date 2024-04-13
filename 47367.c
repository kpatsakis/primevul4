static void flush_dpb(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;
    int i;

    for (i = 0; i <= MAX_DELAYED_PIC_COUNT; i++) {
        if (h->delayed_pic[i])
            h->delayed_pic[i]->reference = 0;
        h->delayed_pic[i] = NULL;
    }

    ff_h264_flush_change(h);

    if (h->DPB)
        for (i = 0; i < H264_MAX_PICTURE_COUNT; i++)
            ff_h264_unref_picture(h, &h->DPB[i]);
    h->cur_pic_ptr = NULL;
    ff_h264_unref_picture(h, &h->cur_pic);

    h->mb_x = h->mb_y = 0;

    h->parse_context.state             = -1;
    h->parse_context.frame_start_found = 0;
    h->parse_context.overread          = 0;
    h->parse_context.overread_index    = 0;
    h->parse_context.index             = 0;
    h->parse_context.last_index        = 0;

    ff_h264_free_tables(h, 1);
    h->context_initialized = 0;
}
