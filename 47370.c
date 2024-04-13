static av_cold int h264_decode_end(AVCodecContext *avctx)
{
    H264Context *h = avctx->priv_data;

    ff_h264_remove_all_refs(h);
    ff_h264_free_context(h);

    ff_h264_unref_picture(h, &h->cur_pic);

    return 0;
}
