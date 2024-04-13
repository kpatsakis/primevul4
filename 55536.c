static int mov_read_vpcc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int version, color_range, color_primaries, color_trc, color_space;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams - 1];

    if (atom.size < 5) {
        av_log(c->fc, AV_LOG_ERROR, "Empty VP Codec Configuration box\n");
        return AVERROR_INVALIDDATA;
    }

    version = avio_r8(pb);
    if (version != 1) {
        av_log(c->fc, AV_LOG_WARNING, "Unsupported VP Codec Configuration box version %d\n", version);
        return 0;
    }
    avio_skip(pb, 3); /* flags */

    avio_skip(pb, 2); /* profile + level */
    color_range     = avio_r8(pb); /* bitDepth, chromaSubsampling, videoFullRangeFlag */
    color_primaries = avio_r8(pb);
    color_trc       = avio_r8(pb);
    color_space     = avio_r8(pb);
    if (avio_rb16(pb)) /* codecIntializationDataSize */
        return AVERROR_INVALIDDATA;

    if (!av_color_primaries_name(color_primaries))
        color_primaries = AVCOL_PRI_UNSPECIFIED;
    if (!av_color_transfer_name(color_trc))
        color_trc = AVCOL_TRC_UNSPECIFIED;
    if (!av_color_space_name(color_space))
        color_space = AVCOL_SPC_UNSPECIFIED;

    st->codecpar->color_range     = (color_range & 1) ? AVCOL_RANGE_JPEG : AVCOL_RANGE_MPEG;
    st->codecpar->color_primaries = color_primaries;
    st->codecpar->color_trc       = color_trc;
    st->codecpar->color_space     = color_space;

    return 0;
}
