static int mxf_handle_missing_index_segment(MXFContext *mxf)
{
    AVFormatContext *s = mxf->fc;
    AVStream *st = NULL;
    MXFIndexTableSegment *segment = NULL;
    MXFPartition *p = NULL;
    int essence_partition_count = 0;
    int i, ret;

    st = mxf_get_opatom_stream(mxf);
    if (!st)
        return 0;

    /* TODO: support raw video without an index if they exist */
    if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO || !is_pcm(st->codecpar->codec_id))
        return 0;

    /* check if file already has a IndexTableSegment */
    for (i = 0; i < mxf->metadata_sets_count; i++) {
        if (mxf->metadata_sets[i]->type == IndexTableSegment)
            return 0;
    }

    /* find the essence partition */
    for (i = 0; i < mxf->partitions_count; i++) {
        /* BodySID == 0 -> no essence */
        if (!mxf->partitions[i].body_sid)
            continue;

        p = &mxf->partitions[i];
        essence_partition_count++;
    }

    /* only handle files with a single essence partition */
    if (essence_partition_count != 1)
        return 0;

    if (!(segment = av_mallocz(sizeof(*segment))))
        return AVERROR(ENOMEM);

    if ((ret = mxf_add_metadata_set(mxf, segment))) {
        mxf_free_metadataset((MXFMetadataSet**)&segment, 1);
        return ret;
    }

    segment->type = IndexTableSegment;
    /* stream will be treated as small EditUnitByteCount */
    segment->edit_unit_byte_count = (av_get_bits_per_sample(st->codecpar->codec_id) * st->codecpar->channels) >> 3;
    segment->index_start_position = 0;
    segment->index_duration = s->streams[0]->duration;
    segment->index_sid = p->index_sid;
    segment->body_sid = p->body_sid;
    return 0;
}
