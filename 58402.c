int attribute_align_arg ff_codec_open2_recursive(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options)
{
    int ret = 0;

    ff_unlock_avcodec(codec);

    ret = avcodec_open2(avctx, codec, options);

    ff_lock_avcodec(avctx, codec);
    return ret;
}
