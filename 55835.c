static int set_channels(AVFormatContext *avctx, AVStream *st, int channels)
{
    if (channels <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Channel count %d invalid.\n", channels);
        return AVERROR_INVALIDDATA;
    }
    st->codecpar->channels       = channels;
    st->codecpar->channel_layout = (st->codecpar->channels == 1) ? AV_CH_LAYOUT_MONO
                                                                 : AV_CH_LAYOUT_STEREO;
    return 0;
}
