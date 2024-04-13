static int mov_read_stsz(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries, sample_size, field_size, num_bytes;
    GetBitContext gb;
    unsigned char* buf;
    int ret;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */

    if (atom.type == MKTAG('s','t','s','z')) {
        sample_size = avio_rb32(pb);
        if (!sc->sample_size) /* do not overwrite value computed in stsd */
            sc->sample_size = sample_size;
        sc->stsz_sample_size = sample_size;
        field_size = 32;
    } else {
        sample_size = 0;
        avio_rb24(pb); /* reserved */
        field_size = avio_r8(pb);
    }
    entries = avio_rb32(pb);

    av_log(c->fc, AV_LOG_TRACE, "sample_size = %u sample_count = %u\n", sc->sample_size, entries);

    sc->sample_count = entries;
    if (sample_size)
        return 0;

    if (field_size != 4 && field_size != 8 && field_size != 16 && field_size != 32) {
        av_log(c->fc, AV_LOG_ERROR, "Invalid sample field size %u\n", field_size);
        return AVERROR_INVALIDDATA;
    }

    if (!entries)
        return 0;
    if (entries >= (UINT_MAX - 4) / field_size)
        return AVERROR_INVALIDDATA;
    if (sc->sample_sizes)
        av_log(c->fc, AV_LOG_WARNING, "Duplicated STSZ atom\n");
    av_free(sc->sample_sizes);
    sc->sample_count = 0;
    sc->sample_sizes = av_malloc_array(entries, sizeof(*sc->sample_sizes));
    if (!sc->sample_sizes)
        return AVERROR(ENOMEM);

    num_bytes = (entries*field_size+4)>>3;

    buf = av_malloc(num_bytes+AV_INPUT_BUFFER_PADDING_SIZE);
    if (!buf) {
        av_freep(&sc->sample_sizes);
        return AVERROR(ENOMEM);
    }

    ret = ffio_read_size(pb, buf, num_bytes);
    if (ret < 0) {
        av_freep(&sc->sample_sizes);
        av_free(buf);
        return ret;
    }

    init_get_bits(&gb, buf, 8*num_bytes);

    for (i = 0; i < entries && !pb->eof_reached; i++) {
        sc->sample_sizes[i] = get_bits_long(&gb, field_size);
        sc->data_size += sc->sample_sizes[i];
    }

    sc->sample_count = i;

    av_free(buf);

    if (pb->eof_reached)
        return AVERROR_EOF;

    return 0;
}
