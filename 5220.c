qeh_out_on_new (void *stream_if_ctx, struct lsquic_stream *stream)
{
    struct qpack_enc_hdl *const qeh = stream_if_ctx;
    qeh->qeh_enc_sm_out = stream;
    if ((qeh->qeh_flags & (QEH_INITIALIZED|QEH_HAVE_SETTINGS))
                                    == (QEH_INITIALIZED|QEH_HAVE_SETTINGS))
        qeh_begin_out(qeh);
    else
        qeh->qeh_conn = lsquic_stream_conn(stream);   /* Or NULL deref in log */
    LSQ_DEBUG("initialized outgoing encoder stream");
    return (void *) qeh;
}