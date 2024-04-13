qeh_out_on_write (struct lsquic_stream *stream, lsquic_stream_ctx_t *ctx)
{
    struct qpack_enc_hdl *const qeh = (void *) ctx;
    struct lsquic_reader reader = {
        .lsqr_read  = lsquic_frab_list_read,
        .lsqr_size  = lsquic_frab_list_size,
        .lsqr_ctx   = &qeh->qeh_fral,
    };
    ssize_t nw;

    nw = lsquic_stream_writef(stream, &reader);
    if (nw >= 0)
    {
        LSQ_DEBUG("wrote %zd bytes to stream", nw);
        (void) lsquic_stream_flush(stream);
        if (lsquic_frab_list_empty(&qeh->qeh_fral))
            lsquic_stream_wantwrite(stream, 0);
    }
    else
    {
        qeh->qeh_conn->cn_if->ci_internal_error(qeh->qeh_conn,
                                            "cannot write to stream");
        LSQ_WARN("cannot write to stream: %s", strerror(errno));
        lsquic_stream_wantwrite(stream, 0);
    }
}