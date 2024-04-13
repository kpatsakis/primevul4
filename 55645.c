static int mxf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    MXFContext *mxf = s->priv_data;
    int ret, size;
    int64_t ret64, pos, next_pos;
    AVStream *st;
    MXFIndexTable *t;
    int edit_units;

    if (mxf->op != OPAtom)
        return mxf_read_packet_old(s, pkt);

    st = mxf_get_opatom_stream(mxf);
    if (!st)
        return AVERROR_EOF;

    /* OPAtom - clip wrapped demuxing */
    /* NOTE: mxf_read_header() makes sure nb_index_tables > 0 for OPAtom */
    t = &mxf->index_tables[0];

    if (mxf->current_edit_unit >= st->duration)
        return AVERROR_EOF;

    edit_units = FFMIN(mxf->edit_units_per_packet, st->duration - mxf->current_edit_unit);

    if ((ret = mxf_edit_unit_absolute_offset(mxf, t, mxf->current_edit_unit, NULL, &pos, 1)) < 0)
        return ret;

    /* compute size by finding the next edit unit or the end of the essence container
     * not pretty, but it works */
    if ((ret = mxf_edit_unit_absolute_offset(mxf, t, mxf->current_edit_unit + edit_units, NULL, &next_pos, 0)) < 0 &&
        (next_pos = mxf_essence_container_end(mxf, t->body_sid)) <= 0) {
        av_log(s, AV_LOG_ERROR, "unable to compute the size of the last packet\n");
        return AVERROR_INVALIDDATA;
    }

    if ((size = next_pos - pos) <= 0) {
        av_log(s, AV_LOG_ERROR, "bad size: %i\n", size);
        return AVERROR_INVALIDDATA;
    }

    if ((ret64 = avio_seek(s->pb, pos, SEEK_SET)) < 0)
        return ret64;

    if ((size = av_get_packet(s->pb, pkt, size)) < 0)
        return size;

    pkt->stream_index = st->index;

    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && t->ptses &&
        mxf->current_edit_unit >= 0 && mxf->current_edit_unit < t->nb_ptses) {
        pkt->dts = mxf->current_edit_unit + t->first_dts;
        pkt->pts = t->ptses[mxf->current_edit_unit];
    } else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
        int ret = mxf_set_audio_pts(mxf, st->codecpar, pkt);
        if (ret < 0)
            return ret;
    }

    mxf->current_edit_unit += edit_units;

    return 0;
}
