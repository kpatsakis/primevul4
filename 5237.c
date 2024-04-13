qeh_maybe_set_user_agent (struct qpack_enc_hdl *qeh,
                                    const struct lsquic_http_headers *headers)
{
    const char *const name = qeh->qeh_exp_rec->qer_flags & QER_SERVER ?
                                    "server" : "user-agent";
    const size_t len = qeh->qeh_exp_rec->qer_flags & QER_SERVER ? 6 : 10;
    int i;

    for (i = 0; i < headers->count; ++i)
        if (len == headers->headers[i].name_len
                && 0 == memcmp(name,
                        lsxpack_header_get_name(&headers->headers[i]), len))
        {
            qeh->qeh_exp_rec->qer_user_agent = strndup(
                            lsxpack_header_get_value(&headers->headers[i]),
                            headers->headers[i].val_len);
            break;
        }
}