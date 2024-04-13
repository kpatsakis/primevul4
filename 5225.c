qeh_in_on_new (void *stream_if_ctx, struct lsquic_stream *stream)
{
    struct qpack_enc_hdl *const qeh = stream_if_ctx;
    qeh->qeh_dec_sm_in = stream;
    if (qeh->qeh_flags & QEH_INITIALIZED)
        lsquic_stream_wantread(qeh->qeh_dec_sm_in, 1);
    else
        qeh->qeh_conn = lsquic_stream_conn(stream);   /* Or NULL deref in log */
    LSQ_DEBUG("initialized incoming decoder stream");
    return (void *) qeh;
}