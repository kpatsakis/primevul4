static int recode_subtitle(AVCodecContext *avctx,
                           AVPacket *outpkt, const AVPacket *inpkt)
{
#if CONFIG_ICONV
    iconv_t cd = (iconv_t)-1;
    int ret = 0;
    char *inb, *outb;
    size_t inl, outl;
    AVPacket tmp;
#endif

    if (avctx->sub_charenc_mode != FF_SUB_CHARENC_MODE_PRE_DECODER || inpkt->size == 0)
        return 0;

#if CONFIG_ICONV
    cd = iconv_open("UTF-8", avctx->sub_charenc);
    av_assert0(cd != (iconv_t)-1);

    inb = inpkt->data;
    inl = inpkt->size;

    if (inl >= INT_MAX / UTF8_MAX_BYTES - AV_INPUT_BUFFER_PADDING_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "Subtitles packet is too big for recoding\n");
        ret = AVERROR(ENOMEM);
        goto end;
    }

    ret = av_new_packet(&tmp, inl * UTF8_MAX_BYTES);
    if (ret < 0)
        goto end;
    outpkt->buf  = tmp.buf;
    outpkt->data = tmp.data;
    outpkt->size = tmp.size;
    outb = outpkt->data;
    outl = outpkt->size;

    if (iconv(cd, &inb, &inl, &outb, &outl) == (size_t)-1 ||
        iconv(cd, NULL, NULL, &outb, &outl) == (size_t)-1 ||
        outl >= outpkt->size || inl != 0) {
        ret = FFMIN(AVERROR(errno), -1);
        av_log(avctx, AV_LOG_ERROR, "Unable to recode subtitle event \"%s\" "
               "from %s to UTF-8\n", inpkt->data, avctx->sub_charenc);
        av_packet_unref(&tmp);
        goto end;
    }
    outpkt->size -= outl;
    memset(outpkt->data + outpkt->size, 0, outl);

end:
    if (cd != (iconv_t)-1)
        iconv_close(cd);
    return ret;
#else
    av_log(avctx, AV_LOG_ERROR, "requesting subtitles recoding without iconv");
    return AVERROR(EINVAL);
#endif
}
