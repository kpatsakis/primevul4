AVCodec *avcodec_find_decoder(enum AVCodecID id)
{
    return find_encdec(id, 0);
}
