static int mov_read_stco(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    avio_r8(pb); /* version */
    avio_rb24(pb); /* flags */

    entries = avio_rb32(pb);

    if (!entries)
        return 0;

    if (sc->chunk_offsets)
        av_log(c->fc, AV_LOG_WARNING, "Duplicated STCO atom\n");
    av_free(sc->chunk_offsets);
    sc->chunk_count = 0;
    sc->chunk_offsets = av_malloc_array(entries, sizeof(*sc->chunk_offsets));
    if (!sc->chunk_offsets)
        return AVERROR(ENOMEM);
    sc->chunk_count = entries;

    if      (atom.type == MKTAG('s','t','c','o'))
        for (i = 0; i < entries && !pb->eof_reached; i++)
            sc->chunk_offsets[i] = avio_rb32(pb);
    else if (atom.type == MKTAG('c','o','6','4'))
        for (i = 0; i < entries && !pb->eof_reached; i++)
            sc->chunk_offsets[i] = avio_rb64(pb);
    else
        return AVERROR_INVALIDDATA;

    sc->chunk_count = i;

    if (pb->eof_reached)
        return AVERROR_EOF;

    return 0;
}
