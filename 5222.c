qeh_read_decoder_stream (void *ctx, const unsigned char *buf, size_t sz,
                                                                    int fin)
{
    struct qpack_enc_hdl *const qeh = (void *) ctx;
    uint64_t offset;
    int s;

    if (fin)
    {
        LSQ_INFO("decoder stream is closed");
        qeh->qeh_conn->cn_if->ci_abort_error(qeh->qeh_conn, 1,
            HEC_CLOSED_CRITICAL_STREAM, "Peer closed QPACK decoder stream");
        goto end;
    }

    offset = lsquic_stream_read_offset(qeh->qeh_dec_sm_in);
    s = lsqpack_enc_decoder_in(&qeh->qeh_encoder, buf, sz);
    if (s != 0)
    {
        LSQ_INFO("error reading decoder stream");
        qeh->qeh_conn->cn_if->ci_abort_error(qeh->qeh_conn, 1,
            HEC_QPACK_DECODER_STREAM_ERROR, "Error interpreting QPACK decoder "
            "stream at offset %"PRIu64, offset);
        goto end;
    }
    LSQ_DEBUG("successfully fed %zu bytes to QPACK decoder", sz);

  end:
    return sz;
}