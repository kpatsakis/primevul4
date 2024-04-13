lsquic_qeh_write_headers (struct qpack_enc_hdl *qeh,
    lsquic_stream_id_t stream_id, unsigned seqno,
    const struct lsquic_http_headers *headers, unsigned char *buf,
    size_t *prefix_sz, size_t *headers_sz, uint64_t *completion_offset,
    enum lsqpack_enc_header_flags *hflags)
{
    if (qeh->qeh_flags & QEH_INITIALIZED)
        return qeh_write_headers(qeh, stream_id, seqno, headers, buf,
                        prefix_sz, headers_sz, completion_offset, hflags);
    else
        return QWH_ERR;
}