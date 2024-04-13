static int mov_read_sidx(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int64_t offset = avio_tell(pb) + atom.size, pts;
    uint8_t version;
    unsigned i, track_id;
    AVStream *st = NULL;
    AVStream *ref_st = NULL;
    MOVStreamContext *sc, *ref_sc = NULL;
    MOVFragmentIndex *index = NULL;
    MOVFragmentIndex **tmp;
    AVRational timescale;

    version = avio_r8(pb);
    if (version > 1) {
        avpriv_request_sample(c->fc, "sidx version %u", version);
        return 0;
    }

    avio_rb24(pb); // flags

    track_id = avio_rb32(pb); // Reference ID
    for (i = 0; i < c->fc->nb_streams; i++) {
        if (c->fc->streams[i]->id == track_id) {
            st = c->fc->streams[i];
            break;
        }
    }
    if (!st) {
        av_log(c->fc, AV_LOG_WARNING, "could not find corresponding track id %d\n", track_id);
        return 0;
    }

    sc = st->priv_data;

    timescale = av_make_q(1, avio_rb32(pb));

    if (timescale.den <= 0) {
        av_log(c->fc, AV_LOG_ERROR, "Invalid sidx timescale 1/%d\n", timescale.den);
        return AVERROR_INVALIDDATA;
    }

    if (version == 0) {
        pts = avio_rb32(pb);
        offset += avio_rb32(pb);
    } else {
        pts = avio_rb64(pb);
        offset += avio_rb64(pb);
    }

    avio_rb16(pb); // reserved

    index = av_mallocz(sizeof(MOVFragmentIndex));
    if (!index)
        return AVERROR(ENOMEM);

    index->track_id = track_id;

    index->item_count = avio_rb16(pb);
    index->items = av_mallocz_array(index->item_count, sizeof(MOVFragmentIndexItem));

    if (!index->items) {
        av_freep(&index);
        return AVERROR(ENOMEM);
    }

    for (i = 0; i < index->item_count; i++) {
        uint32_t size = avio_rb32(pb);
        uint32_t duration = avio_rb32(pb);
        if (size & 0x80000000) {
            avpriv_request_sample(c->fc, "sidx reference_type 1");
            av_freep(&index->items);
            av_freep(&index);
            return AVERROR_PATCHWELCOME;
        }
        avio_rb32(pb); // sap_flags
        index->items[i].moof_offset = offset;
        index->items[i].time = av_rescale_q(pts, st->time_base, timescale);
        offset += size;
        pts += duration;
    }

    st->duration = sc->track_end = pts;

    tmp = av_realloc_array(c->fragment_index_data,
                           c->fragment_index_count + 1,
                           sizeof(MOVFragmentIndex*));
    if (!tmp) {
        av_freep(&index->items);
        av_freep(&index);
        return AVERROR(ENOMEM);
    }

    c->fragment_index_data = tmp;
    c->fragment_index_data[c->fragment_index_count++] = index;
    sc->has_sidx = 1;

    if (offset == avio_size(pb)) {
        for (i = 0; i < c->fc->nb_streams; i++) {
            if (c->fc->streams[i]->id == c->fragment_index_data[0]->track_id) {
                ref_st = c->fc->streams[i];
                ref_sc = ref_st->priv_data;
                break;
            }
        }
        for (i = 0; i < c->fc->nb_streams; i++) {
            st = c->fc->streams[i];
            sc = st->priv_data;
            if (!sc->has_sidx) {
                st->duration = sc->track_end = av_rescale(ref_st->duration, sc->time_scale, ref_sc->time_scale);
            }
        }

        c->fragment_index_complete = 1;
    }

    return 0;
}
