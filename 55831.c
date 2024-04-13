static int parse_global_var(AVFormatContext *avctx, AVStream *st,
                            const char *name, int size)
{
    MvContext *mv = avctx->priv_data;
    AVIOContext *pb = avctx->pb;
    if (!strcmp(name, "__NUM_I_TRACKS")) {
        mv->nb_video_tracks = var_read_int(pb, size);
    } else if (!strcmp(name, "__NUM_A_TRACKS")) {
        mv->nb_audio_tracks = var_read_int(pb, size);
    } else if (!strcmp(name, "COMMENT") || !strcmp(name, "TITLE")) {
        var_read_metadata(avctx, name, size);
    } else if (!strcmp(name, "LOOP_MODE") || !strcmp(name, "NUM_LOOPS") ||
               !strcmp(name, "OPTIMIZED")) {
        avio_skip(pb, size); // ignore
    } else
        return AVERROR_INVALIDDATA;

    return 0;
}
