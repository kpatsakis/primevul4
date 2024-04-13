static int mxf_compute_index_tables(MXFContext *mxf)
{
    int i, j, k, ret, nb_sorted_segments;
    MXFIndexTableSegment **sorted_segments = NULL;
    AVStream *st = NULL;

    for (i = 0; i < mxf->fc->nb_streams; i++) {
        if (mxf->fc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_DATA)
            continue;
        st = mxf->fc->streams[i];
        break;
    }

    if ((ret = mxf_get_sorted_table_segments(mxf, &nb_sorted_segments, &sorted_segments)) ||
        nb_sorted_segments <= 0) {
        av_log(mxf->fc, AV_LOG_WARNING, "broken or empty index\n");
        return 0;
    }

    /* sanity check and count unique BodySIDs/IndexSIDs */
    for (i = 0; i < nb_sorted_segments; i++) {
        if (i == 0 || sorted_segments[i-1]->index_sid != sorted_segments[i]->index_sid)
            mxf->nb_index_tables++;
        else if (sorted_segments[i-1]->body_sid != sorted_segments[i]->body_sid) {
            av_log(mxf->fc, AV_LOG_ERROR, "found inconsistent BodySID\n");
            ret = AVERROR_INVALIDDATA;
            goto finish_decoding_index;
        }
    }

    mxf->index_tables = av_mallocz_array(mxf->nb_index_tables,
                                         sizeof(*mxf->index_tables));
    if (!mxf->index_tables) {
        av_log(mxf->fc, AV_LOG_ERROR, "failed to allocate index tables\n");
        ret = AVERROR(ENOMEM);
        goto finish_decoding_index;
    }

    /* distribute sorted segments to index tables */
    for (i = j = 0; i < nb_sorted_segments; i++) {
        if (i != 0 && sorted_segments[i-1]->index_sid != sorted_segments[i]->index_sid) {
            /* next IndexSID */
            j++;
        }

        mxf->index_tables[j].nb_segments++;
    }

    for (i = j = 0; j < mxf->nb_index_tables; i += mxf->index_tables[j++].nb_segments) {
        MXFIndexTable *t = &mxf->index_tables[j];

        t->segments = av_mallocz_array(t->nb_segments,
                                       sizeof(*t->segments));

        if (!t->segments) {
            av_log(mxf->fc, AV_LOG_ERROR, "failed to allocate IndexTableSegment"
                   " pointer array\n");
            ret = AVERROR(ENOMEM);
            goto finish_decoding_index;
        }

        if (sorted_segments[i]->index_start_position)
            av_log(mxf->fc, AV_LOG_WARNING, "IndexSID %i starts at EditUnit %"PRId64" - seeking may not work as expected\n",
                   sorted_segments[i]->index_sid, sorted_segments[i]->index_start_position);

        memcpy(t->segments, &sorted_segments[i], t->nb_segments * sizeof(MXFIndexTableSegment*));
        t->index_sid = sorted_segments[i]->index_sid;
        t->body_sid = sorted_segments[i]->body_sid;

        if ((ret = mxf_compute_ptses_fake_index(mxf, t)) < 0)
            goto finish_decoding_index;

        /* fix zero IndexDurations */
        for (k = 0; k < t->nb_segments; k++) {
            if (t->segments[k]->index_duration)
                continue;

            if (t->nb_segments > 1)
                av_log(mxf->fc, AV_LOG_WARNING, "IndexSID %i segment %i has zero IndexDuration and there's more than one segment\n",
                       t->index_sid, k);

            if (!st) {
                av_log(mxf->fc, AV_LOG_WARNING, "no streams?\n");
                break;
            }

            /* assume the first stream's duration is reasonable
             * leave index_duration = 0 on further segments in case we have any (unlikely)
             */
            t->segments[k]->index_duration = st->duration;
            break;
        }
    }

    ret = 0;
finish_decoding_index:
    av_free(sorted_segments);
    return ret;
}
