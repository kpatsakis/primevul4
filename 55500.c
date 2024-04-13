static int mov_read_extradata(MOVContext *c, AVIOContext *pb, MOVAtom atom,
                              enum AVCodecID codec_id)
{
    AVStream *st;
    uint64_t original_size;
    int err;

    if (c->fc->nb_streams < 1) // will happen with jp2 files
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if (st->codecpar->codec_id != codec_id)
        return 0; /* unexpected codec_id - don't mess with extradata */

    original_size = st->codecpar->extradata_size;
    err = mov_realloc_extradata(st->codecpar, atom);
    if (err)
        return err;

    err =  mov_read_atom_into_extradata(c, pb, atom, st->codecpar,  st->codecpar->extradata + original_size);
    if (err < 0)
        return err;
    return 0; // Note: this is the original behavior to ignore truncation.
}
