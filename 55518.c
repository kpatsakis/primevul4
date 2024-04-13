static int mov_read_senc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    size_t auxiliary_info_size;

    if (c->decryption_key_len == 0 || c->fc->nb_streams < 1)
        return 0;

    st = c->fc->streams[c->fc->nb_streams - 1];
    sc = st->priv_data;

    if (sc->cenc.aes_ctr) {
        av_log(c->fc, AV_LOG_ERROR, "duplicate senc atom\n");
        return AVERROR_INVALIDDATA;
    }

    avio_r8(pb); /* version */
    sc->cenc.use_subsamples = avio_rb24(pb) & 0x02; /* flags */

    avio_rb32(pb);        /* entries */

    if (atom.size < 8 || atom.size > FFMIN(INT_MAX, SIZE_MAX)) {
        av_log(c->fc, AV_LOG_ERROR, "senc atom size %"PRId64" invalid\n", atom.size);
        return AVERROR_INVALIDDATA;
    }

    /* save the auxiliary info as is */
    auxiliary_info_size = atom.size - 8;

    sc->cenc.auxiliary_info = av_malloc(auxiliary_info_size);
    if (!sc->cenc.auxiliary_info) {
        return AVERROR(ENOMEM);
    }

    sc->cenc.auxiliary_info_end = sc->cenc.auxiliary_info + auxiliary_info_size;
    sc->cenc.auxiliary_info_pos = sc->cenc.auxiliary_info;
    sc->cenc.auxiliary_info_index = 0;

    if (avio_read(pb, sc->cenc.auxiliary_info, auxiliary_info_size) != auxiliary_info_size) {
        av_log(c->fc, AV_LOG_ERROR, "failed to read the auxiliary info");
        return AVERROR_INVALIDDATA;
    }

    /* initialize the cipher */
    sc->cenc.aes_ctr = av_aes_ctr_alloc();
    if (!sc->cenc.aes_ctr) {
        return AVERROR(ENOMEM);
    }

    return av_aes_ctr_init(sc->cenc.aes_ctr, c->decryption_key);
}
