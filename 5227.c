qeh_in_on_read (struct lsquic_stream *stream, lsquic_stream_ctx_t *ctx)
{
    struct qpack_enc_hdl *const qeh = (void *) ctx;
    ssize_t nread;

    nread = lsquic_stream_readf(stream, qeh_read_decoder_stream, qeh);
    if (nread <= 0)
    {
        if (nread < 0)
        {
            LSQ_WARN("cannot read from encoder stream: %s", strerror(errno));
            qeh->qeh_conn->cn_if->ci_internal_error(qeh->qeh_conn,
                                        "cannot read from encoder stream");
        }
        else
        {
            LSQ_INFO("encoder stream closed by peer: abort connection");
            qeh->qeh_conn->cn_if->ci_abort_error(qeh->qeh_conn, 1,
                HEC_CLOSED_CRITICAL_STREAM, "encoder stream closed");
        }
        lsquic_stream_wantread(stream, 0);
    }
}