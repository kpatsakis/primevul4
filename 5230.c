qeh_out_on_close (struct lsquic_stream *stream, lsquic_stream_ctx_t *ctx)
{
    struct qpack_enc_hdl *const qeh = (void *) ctx;
    qeh->qeh_enc_sm_out = NULL;
    LSQ_DEBUG("closed outgoing encoder stream");
}