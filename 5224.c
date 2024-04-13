qeh_in_on_close (struct lsquic_stream *stream, lsquic_stream_ctx_t *ctx)
{
    struct qpack_enc_hdl *const qeh = (void *) ctx;
    LSQ_DEBUG("closed incoming decoder stream");
    qeh->qeh_dec_sm_in = NULL;
}