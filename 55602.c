static int is_pcm(enum AVCodecID codec_id)
{
    /* we only care about "normal" PCM codecs until we get samples */
    return codec_id >= AV_CODEC_ID_PCM_S16LE && codec_id < AV_CODEC_ID_PCM_S24DAUD;
}
