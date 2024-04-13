qeh_write_headers (struct qpack_enc_hdl *qeh, lsquic_stream_id_t stream_id,
    unsigned seqno, const struct lsquic_http_headers *headers,
    unsigned char *buf, size_t *prefix_sz, size_t *headers_sz,
    uint64_t *completion_offset, enum lsqpack_enc_header_flags *hflags)
{
    unsigned char *p = buf;
    unsigned char *const end = buf + *headers_sz;
    const unsigned char *enc_p;
    size_t enc_sz, hea_sz, total_enc_sz;
    ssize_t nw;
    enum lsqpack_enc_status st;
    int i, s, write_to_stream;
    enum lsqpack_enc_flags enc_flags;
    enum qwh_status retval;
#ifndef WIN32
    unsigned char enc_buf[ qeh->qeh_encoder.qpe_cur_max_capacity * 2 ];
#else
    unsigned char *enc_buf;
    enc_buf = _malloca(qeh->qeh_encoder.qpe_cur_max_capacity * 2);
    if (!enc_buf)
        return QWH_ERR;
#endif

    if (qeh->qeh_exp_rec)
    {
        const lsquic_time_t now = lsquic_time_now();
        if (qeh->qeh_exp_rec->qer_hblock_count == 0)
            qeh->qeh_exp_rec->qer_first_req = now;
        qeh->qeh_exp_rec->qer_last_req = now;
        ++qeh->qeh_exp_rec->qer_hblock_count;
        if (!qeh->qeh_exp_rec->qer_user_agent)
            qeh_maybe_set_user_agent(qeh, headers);
    }

    s = lsqpack_enc_start_header(&qeh->qeh_encoder, stream_id, 0);
    if (s != 0)
    {
        LSQ_WARN("cannot start header");
        retval = QWH_ERR;
        goto end;
    }
    LSQ_DEBUG("begin encoding headers for stream %"PRIu64, stream_id);

    if (qeh->qeh_enc_sm_out)
        enc_flags = 0;
    else
    {
        enc_flags = LQEF_NO_INDEX;
        LSQ_DEBUG("encoder stream is unavailable, won't index headers");
    }
    write_to_stream = qeh->qeh_enc_sm_out
                                && lsquic_frab_list_empty(&qeh->qeh_fral);
    total_enc_sz = 0;
    for (i = 0; i < headers->count; ++i)
    {
        if (headers->headers[i].buf == NULL)
            continue;
        enc_sz = sizeof(enc_buf);
        hea_sz = end - p;
        st = lsqpack_enc_encode(&qeh->qeh_encoder, enc_buf, &enc_sz, p,
                                &hea_sz, &headers->headers[i], enc_flags);
        switch (st)
        {
        case LQES_OK:
            LSQ_DEBUG("encoded `%.*s': `%.*s' -- %zd bytes to header block, "
                "%zd bytes to encoder stream",
                (int) headers->headers[i].name_len,
                    lsxpack_header_get_name(&headers->headers[i]),
                (int) headers->headers[i].val_len,
                    lsxpack_header_get_value(&headers->headers[i]),
                hea_sz, enc_sz);
            total_enc_sz += enc_sz;
            p += hea_sz;
            if (enc_sz)
            {
                if (write_to_stream)
                {
                    nw = lsquic_stream_write(qeh->qeh_enc_sm_out, enc_buf, enc_sz);
                    if ((size_t) nw == enc_sz)
                        break;
                    if (nw < 0)
                    {
                        LSQ_INFO("could not write to encoder stream: %s",
                                                                strerror(errno));
                        retval = QWH_ERR;
                        goto end;
                    }
                    write_to_stream = 0;
                    enc_p = enc_buf + (size_t) nw;
                    enc_sz -= (size_t) nw;
                }
                else
                    enc_p = enc_buf;
                if (0 != lsquic_frab_list_write(&qeh->qeh_fral, enc_p, enc_sz))
                {
                    LSQ_INFO("could not write to frab list");
                    retval = QWH_ERR;
                    goto end;
                }
            }
            break;
        case LQES_NOBUF_HEAD:
            retval = QWH_ENOBUF;
            goto end;
        default:
            assert(0);
            retval = QWH_ERR;
            goto end;
        case LQES_NOBUF_ENC:
            LSQ_DEBUG("not enough room to write encoder stream data");
            retval = QWH_ERR;
            goto end;
        }
    }

    nw = lsqpack_enc_end_header(&qeh->qeh_encoder, buf - *prefix_sz,
                                                        *prefix_sz, hflags);
    if (nw <= 0)
    {
        LSQ_WARN("could not end header: %zd", nw);
        retval = QWH_ERR;
        goto end;
    }

    if ((size_t) nw < *prefix_sz)
    {
        memmove(buf - nw, buf - *prefix_sz, (size_t) nw);
        *prefix_sz = (size_t) nw;
    }
    *headers_sz = p - buf;
    if (qeh->qeh_exp_rec)
        qeh->qeh_exp_rec->qer_hblock_size += p - buf;
    if (lsquic_frab_list_empty(&qeh->qeh_fral))
    {
        LSQ_DEBUG("all %zd bytes of encoder stream written out; header block "
            "is %zd bytes; estimated compression ratio %.3f", total_enc_sz,
            *headers_sz, lsqpack_enc_ratio(&qeh->qeh_encoder));
        retval = QWH_FULL;
        goto end;
    }
    else
    {
        *completion_offset = lsquic_qeh_enc_off(qeh)
                                    + lsquic_frab_list_size(&qeh->qeh_fral);
        LSQ_DEBUG("not all %zd bytes of encoder stream written out; %zd bytes "
            "buffered; header block is %zd bytes; estimated compression ratio "
            "%.3f", total_enc_sz, lsquic_frab_list_size(&qeh->qeh_fral),
            *headers_sz, lsqpack_enc_ratio(&qeh->qeh_encoder));
        retval = QWH_PARTIAL;
        goto end;
    }

  end:
#ifdef WIN32
    _freea(enc_buf);
#endif
    return retval;
}