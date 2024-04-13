static int parse_audio_var(AVFormatContext *avctx, AVStream *st,
                           const char *name, int size)
{
    MvContext *mv = avctx->priv_data;
    AVIOContext *pb = avctx->pb;
    if (!strcmp(name, "__DIR_COUNT")) {
        st->nb_frames = var_read_int(pb, size);
    } else if (!strcmp(name, "AUDIO_FORMAT")) {
        mv->aformat = var_read_int(pb, size);
    } else if (!strcmp(name, "COMPRESSION")) {
        mv->acompression = var_read_int(pb, size);
    } else if (!strcmp(name, "DEFAULT_VOL")) {
        var_read_metadata(avctx, name, size);
    } else if (!strcmp(name, "NUM_CHANNELS")) {
        return set_channels(avctx, st, var_read_int(pb, size));
    } else if (!strcmp(name, "SAMPLE_RATE")) {
        st->codecpar->sample_rate = var_read_int(pb, size);
        avpriv_set_pts_info(st, 33, 1, st->codecpar->sample_rate);
    } else if (!strcmp(name, "SAMPLE_WIDTH")) {
        st->codecpar->bits_per_coded_sample = var_read_int(pb, size) * 8;
    } else
        return AVERROR_INVALIDDATA;

    return 0;
}
