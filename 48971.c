kg_seal_iov_length(OM_uint32 *minor_status,
                   gss_ctx_id_t context_handle,
                   int conf_req_flag,
                   gss_qop_t qop_req,
                   int *conf_state,
                   gss_iov_buffer_desc *iov,
                   int iov_count,
                   int toktype)
{
    krb5_gss_ctx_id_rec *ctx;
    gss_iov_buffer_t header, trailer, padding;
    size_t data_length, assoc_data_length;
    size_t gss_headerlen, gss_padlen, gss_trailerlen;
    unsigned int k5_headerlen = 0, k5_trailerlen = 0, k5_padlen = 0;
    krb5_error_code code;
    krb5_context context;
    int dce_or_mic;

    if (qop_req != GSS_C_QOP_DEFAULT) {
        *minor_status = (OM_uint32)G_UNKNOWN_QOP;
        return GSS_S_FAILURE;
    }

    ctx = (krb5_gss_ctx_id_rec *)context_handle;
    if (!ctx->established) {
        *minor_status = KG_CTX_INCOMPLETE;
        return GSS_S_NO_CONTEXT;
    }

    header = kg_locate_header_iov(iov, iov_count, toktype);
    if (header == NULL) {
        *minor_status = EINVAL;
        return GSS_S_FAILURE;
    }
    INIT_IOV_DATA(header);

    trailer = kg_locate_iov(iov, iov_count, GSS_IOV_BUFFER_TYPE_TRAILER);
    if (trailer != NULL) {
        INIT_IOV_DATA(trailer);
    }

    /* MIC tokens and DCE-style wrap tokens have similar length considerations:
     * no padding, and the framing surrounds the header only, not the data. */
    dce_or_mic = ((ctx->gss_flags & GSS_C_DCE_STYLE) != 0 ||
                  toktype == KG_TOK_MIC_MSG);

    /* For CFX, EC is used instead of padding, and is placed in header or trailer */
    padding = kg_locate_iov(iov, iov_count, GSS_IOV_BUFFER_TYPE_PADDING);
    if (padding == NULL) {
        if (conf_req_flag && ctx->proto == 0 && !dce_or_mic) {
            *minor_status = EINVAL;
            return GSS_S_FAILURE;
        }
    } else {
        INIT_IOV_DATA(padding);
    }

    kg_iov_msglen(iov, iov_count, &data_length, &assoc_data_length);

    if (conf_req_flag && kg_integ_only_iov(iov, iov_count))
        conf_req_flag = FALSE;

    context = ctx->k5_context;

    gss_headerlen = gss_padlen = gss_trailerlen = 0;

    if (ctx->proto == 1) {
        krb5_key key;
        krb5_enctype enctype;
        size_t ec;

        key = (ctx->have_acceptor_subkey) ? ctx->acceptor_subkey : ctx->subkey;
        enctype = key->keyblock.enctype;

        code = krb5_c_crypto_length(context, enctype,
                                    conf_req_flag ?
                                    KRB5_CRYPTO_TYPE_TRAILER : KRB5_CRYPTO_TYPE_CHECKSUM,
                                    &k5_trailerlen);
        if (code != 0) {
            *minor_status = code;
            return GSS_S_FAILURE;
        }

        if (conf_req_flag) {
            code = krb5_c_crypto_length(context, enctype, KRB5_CRYPTO_TYPE_HEADER, &k5_headerlen);
            if (code != 0) {
                *minor_status = code;
                return GSS_S_FAILURE;
            }
        }

        gss_headerlen = 16; /* Header */
        if (conf_req_flag) {
            gss_headerlen += k5_headerlen; /* Kerb-Header */
            gss_trailerlen = 16 /* E(Header) */ + k5_trailerlen; /* Kerb-Trailer */

            code = krb5_c_padding_length(context, enctype,
                                         data_length - assoc_data_length + 16 /* E(Header) */, &k5_padlen);
            if (code != 0) {
                *minor_status = code;
                return GSS_S_FAILURE;
            }

            if (k5_padlen == 0 && dce_or_mic) {
                /* Windows rejects AEAD tokens with non-zero EC */
                code = krb5_c_block_size(context, enctype, &ec);
                if (code != 0) {
                    *minor_status = code;
                    return GSS_S_FAILURE;
                }
            } else
                ec = k5_padlen;

            gss_trailerlen += ec;
        } else {
            gss_trailerlen = k5_trailerlen; /* Kerb-Checksum */
        }
    } else if (!dce_or_mic) {
        k5_padlen = (ctx->sealalg == SEAL_ALG_MICROSOFT_RC4) ? 1 : 8;

        if (k5_padlen == 1)
            gss_padlen = 1;
        else
            gss_padlen = k5_padlen - ((data_length - assoc_data_length) % k5_padlen);
    }

    data_length += gss_padlen;

    if (ctx->proto == 0) {
        /* Header | Checksum | Confounder | Data | Pad */
        size_t data_size;

        k5_headerlen = kg_confounder_size(context, ctx->enc->keyblock.enctype);

        data_size = 14 /* Header */ + ctx->cksum_size + k5_headerlen;

        if (!dce_or_mic)
            data_size += data_length;

        gss_headerlen = g_token_size(ctx->mech_used, data_size);

        /* g_token_size() will include data_size as well as the overhead, so
         * subtract data_length just to get the overhead (ie. token size) */
        if (!dce_or_mic)
            gss_headerlen -= data_length;
    }

    if (minor_status != NULL)
        *minor_status = 0;

    if (trailer == NULL)
        gss_headerlen += gss_trailerlen;
    else
        trailer->buffer.length = gss_trailerlen;

    assert(gss_padlen == 0 || padding != NULL);

    if (padding != NULL)
        padding->buffer.length = gss_padlen;

    header->buffer.length = gss_headerlen;

    if (conf_state != NULL)
        *conf_state = conf_req_flag;

    return GSS_S_COMPLETE;
}
