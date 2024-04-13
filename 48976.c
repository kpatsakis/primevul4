make_seal_token_v1_iov(krb5_context context,
                       krb5_gss_ctx_id_rec *ctx,
                       int conf_req_flag,
                       int *conf_state,
                       gss_iov_buffer_desc *iov,
                       int iov_count,
                       int toktype)
{
    krb5_error_code code = 0;
    gss_iov_buffer_t header;
    gss_iov_buffer_t padding;
    gss_iov_buffer_t trailer;
    krb5_checksum md5cksum;
    krb5_checksum cksum;
    size_t k5_headerlen = 0, k5_trailerlen = 0;
    size_t data_length = 0, assoc_data_length = 0;
    size_t tmsglen = 0, tlen;
    unsigned char *ptr;
    krb5_keyusage sign_usage = KG_USAGE_SIGN;

    md5cksum.length = cksum.length = 0;
    md5cksum.contents = cksum.contents = NULL;

    header = kg_locate_header_iov(iov, iov_count, toktype);
    if (header == NULL)
        return EINVAL;

    padding = kg_locate_iov(iov, iov_count, GSS_IOV_BUFFER_TYPE_PADDING);
    if (padding == NULL && toktype == KG_TOK_WRAP_MSG &&
        (ctx->gss_flags & GSS_C_DCE_STYLE) == 0)
        return EINVAL;

    trailer = kg_locate_iov(iov, iov_count, GSS_IOV_BUFFER_TYPE_TRAILER);
    if (trailer != NULL)
        trailer->buffer.length = 0;

    /* Determine confounder length */
    if (toktype == KG_TOK_WRAP_MSG || conf_req_flag)
        k5_headerlen = kg_confounder_size(context, ctx->enc->keyblock.enctype);

    /* Check padding length */
    if (toktype == KG_TOK_WRAP_MSG) {
        size_t k5_padlen = (ctx->sealalg == SEAL_ALG_MICROSOFT_RC4) ? 1 : 8;
        size_t gss_padlen;
        size_t conf_data_length;

        kg_iov_msglen(iov, iov_count, &data_length, &assoc_data_length);
        conf_data_length = k5_headerlen + data_length - assoc_data_length;

        if (k5_padlen == 1)
            gss_padlen = 1; /* one byte to indicate one byte of padding */
        else
            gss_padlen = k5_padlen - (conf_data_length % k5_padlen);

        if (ctx->gss_flags & GSS_C_DCE_STYLE) {
            /* DCE will pad the actual data itself; padding buffer optional and will be zeroed */
            gss_padlen = 0;

            if (conf_data_length % k5_padlen)
                code = KRB5_BAD_MSIZE;
        } else if (padding->type & GSS_IOV_BUFFER_FLAG_ALLOCATE) {
            code = kg_allocate_iov(padding, gss_padlen);
        } else if (padding->buffer.length < gss_padlen) {
            code = KRB5_BAD_MSIZE;
        }
        if (code != 0)
            goto cleanup;

        /* Initialize padding buffer to pad itself */
        if (padding != NULL) {
            padding->buffer.length = gss_padlen;
            memset(padding->buffer.value, (int)gss_padlen, gss_padlen);
        }

        if (ctx->gss_flags & GSS_C_DCE_STYLE)
            tmsglen = k5_headerlen; /* confounder length */
        else
            tmsglen = conf_data_length + padding->buffer.length;
    }

    /* Determine token size */
    tlen = g_token_size(ctx->mech_used, 14 + ctx->cksum_size + tmsglen);

    k5_headerlen += tlen - tmsglen;

    if (header->type & GSS_IOV_BUFFER_FLAG_ALLOCATE)
        code = kg_allocate_iov(header, k5_headerlen);
    else if (header->buffer.length < k5_headerlen)
        code = KRB5_BAD_MSIZE;
    if (code != 0)
        goto cleanup;

    header->buffer.length = k5_headerlen;

    ptr = (unsigned char *)header->buffer.value;
    g_make_token_header(ctx->mech_used, 14 + ctx->cksum_size + tmsglen, &ptr, toktype);

    /* 0..1 SIGN_ALG */
    store_16_le(ctx->signalg, &ptr[0]);

    /* 2..3 SEAL_ALG or Filler */
    if (toktype == KG_TOK_WRAP_MSG && conf_req_flag) {
        store_16_le(ctx->sealalg, &ptr[2]);
    } else {
        /* No seal */
        ptr[2] = 0xFF;
        ptr[3] = 0xFF;
    }

    /* 4..5 Filler */
    ptr[4] = 0xFF;
    ptr[5] = 0xFF;

    /* pad the plaintext, encrypt if needed, and stick it in the token */

    /* initialize the checksum */
    switch (ctx->signalg) {
    case SGN_ALG_DES_MAC_MD5:
    case SGN_ALG_MD2_5:
        md5cksum.checksum_type = CKSUMTYPE_RSA_MD5;
        break;
    case SGN_ALG_HMAC_SHA1_DES3_KD:
        md5cksum.checksum_type = CKSUMTYPE_HMAC_SHA1_DES3;
        break;
    case SGN_ALG_HMAC_MD5:
        md5cksum.checksum_type = CKSUMTYPE_HMAC_MD5_ARCFOUR;
        if (toktype != KG_TOK_WRAP_MSG)
            sign_usage = 15;
        break;
    default:
    case SGN_ALG_DES_MAC:
        abort ();
    }

    code = krb5_c_checksum_length(context, md5cksum.checksum_type, &k5_trailerlen);
    if (code != 0)
        goto cleanup;
    md5cksum.length = k5_trailerlen;

    if (k5_headerlen != 0 && toktype == KG_TOK_WRAP_MSG) {
        code = kg_make_confounder(context, ctx->enc->keyblock.enctype,
                                  ptr + 14 + ctx->cksum_size);
        if (code != 0)
            goto cleanup;
    }

    /* compute the checksum */
    code = kg_make_checksum_iov_v1(context, md5cksum.checksum_type,
                                   ctx->cksum_size, ctx->seq, ctx->enc,
                                   sign_usage, iov, iov_count, toktype,
                                   &md5cksum);
    if (code != 0)
        goto cleanup;

    switch (ctx->signalg) {
    case SGN_ALG_DES_MAC_MD5:
    case SGN_ALG_3:
        code = kg_encrypt_inplace(context, ctx->seq, KG_USAGE_SEAL,
                                  (g_OID_equal(ctx->mech_used,
                                               gss_mech_krb5_old) ?
                                   ctx->seq->keyblock.contents : NULL),
                                  md5cksum.contents, 16);
        if (code != 0)
            goto cleanup;

        cksum.length = ctx->cksum_size;
        cksum.contents = md5cksum.contents + 16 - cksum.length;

        memcpy(ptr + 14, cksum.contents, cksum.length);
        break;
    case SGN_ALG_HMAC_SHA1_DES3_KD:
        assert(md5cksum.length == ctx->cksum_size);
        memcpy(ptr + 14, md5cksum.contents, md5cksum.length);
        break;
    case SGN_ALG_HMAC_MD5:
        memcpy(ptr + 14, md5cksum.contents, ctx->cksum_size);
        break;
    }

    /* create the seq_num */
    code = kg_make_seq_num(context, ctx->seq, ctx->initiate ? 0 : 0xFF,
                           (OM_uint32)ctx->seq_send, ptr + 14, ptr + 6);
    if (code != 0)
        goto cleanup;

    if (conf_req_flag) {
        if (ctx->sealalg == SEAL_ALG_MICROSOFT_RC4) {
            unsigned char bigend_seqnum[4];
            krb5_keyblock *enc_key;
            size_t i;

            store_32_be(ctx->seq_send, bigend_seqnum);

            code = krb5_k_key_keyblock(context, ctx->enc, &enc_key);
            if (code != 0)
                goto cleanup;

            assert(enc_key->length == 16);

            for (i = 0; i < enc_key->length; i++)
                ((char *)enc_key->contents)[i] ^= 0xF0;

            code = kg_arcfour_docrypt_iov(context, enc_key, 0,
                                          bigend_seqnum, 4,
                                          iov, iov_count);
            krb5_free_keyblock(context, enc_key);
        } else {
            code = kg_encrypt_iov(context, ctx->proto,
                                  ((ctx->gss_flags & GSS_C_DCE_STYLE) != 0),
                                  0 /*EC*/, 0 /*RRC*/,
                                  ctx->enc, KG_USAGE_SEAL, NULL,
                                  iov, iov_count);
        }
        if (code != 0)
            goto cleanup;
    }

    ctx->seq_send++;
    ctx->seq_send &= 0xFFFFFFFFL;

    code = 0;

    if (conf_state != NULL)
        *conf_state = conf_req_flag;

cleanup:
    if (code != 0)
        kg_release_iov(iov, iov_count);
    krb5_free_checksum_contents(context, &md5cksum);

    return code;
}
