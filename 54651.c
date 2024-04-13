static av_cold int decode_init(AVCodecContext *avctx)
{
    SmackVContext * const c = avctx->priv_data;
    int ret;

    c->avctx = avctx;

    avctx->pix_fmt = AV_PIX_FMT_PAL8;

    c->pic = av_frame_alloc();
    if (!c->pic)
        return AVERROR(ENOMEM);

    /* decode huffman trees from extradata */
    if(avctx->extradata_size < 16){
        av_log(avctx, AV_LOG_ERROR, "Extradata missing!\n");
        return AVERROR_INVALIDDATA;
    }

    if ((ret = decode_header_trees(c))) {
        decode_end(avctx);
        return ret;
    }

    return 0;
}
