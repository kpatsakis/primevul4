int mbedtls_ssl_encrypt_buf( mbedtls_ssl_context *ssl,
                             mbedtls_ssl_transform *transform,
                             mbedtls_record *rec,
                             int (*f_rng)(void *, unsigned char *, size_t),
                             void *p_rng )
{
    mbedtls_cipher_mode_t mode;
    int auth_done = 0;
    unsigned char * data;
    unsigned char add_data[13 + 1 + MBEDTLS_SSL_CID_OUT_LEN_MAX ];
    size_t add_data_len;
    size_t post_avail;

    /* The SSL context is only used for debugging purposes! */
#if !defined(MBEDTLS_DEBUG_C)
    ssl = NULL; /* make sure we don't use it except for debug */
    ((void) ssl);
#endif

    /* The PRNG is used for dynamic IV generation that's used
     * for CBC transformations in TLS 1.1 and TLS 1.2. */
#if !( defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC) && \
       ( defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2) ) )
    ((void) f_rng);
    ((void) p_rng);
#endif

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> encrypt buf" ) );

    if( transform == NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "no transform provided to encrypt_buf" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }
    if( rec == NULL
        || rec->buf == NULL
        || rec->buf_len < rec->data_offset
        || rec->buf_len - rec->data_offset < rec->data_len
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        || rec->cid_len != 0
#endif
        )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad record structure provided to encrypt_buf" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    data = rec->buf + rec->data_offset;
    post_avail = rec->buf_len - ( rec->data_len + rec->data_offset );
    MBEDTLS_SSL_DEBUG_BUF( 4, "before encrypt: output payload",
                           data, rec->data_len );

    mode = mbedtls_cipher_get_cipher_mode( &transform->cipher_ctx_enc );

    if( rec->data_len > MBEDTLS_SSL_OUT_CONTENT_LEN )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "Record content %" MBEDTLS_PRINTF_SIZET
                                    " too large, maximum %" MBEDTLS_PRINTF_SIZET,
                                    rec->data_len,
                                    (size_t) MBEDTLS_SSL_OUT_CONTENT_LEN ) );
        return( MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
    }

    /* The following two code paths implement the (D)TLSInnerPlaintext
     * structure present in TLS 1.3 and DTLS 1.2 + CID.
     *
     * See ssl_build_inner_plaintext() for more information.
     *
     * Note that this changes `rec->data_len`, and hence
     * `post_avail` needs to be recalculated afterwards.
     *
     * Note also that the two code paths cannot occur simultaneously
     * since they apply to different versions of the protocol. There
     * is hence no risk of double-addition of the inner plaintext.
     */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    if( transform->minor_ver == MBEDTLS_SSL_MINOR_VERSION_4 )
    {
        size_t padding =
            ssl_compute_padding_length( rec->data_len,
                                        MBEDTLS_SSL_TLS1_3_PADDING_GRANULARITY );
        if( ssl_build_inner_plaintext( data,
                                       &rec->data_len,
                                       post_avail,
                                       rec->type,
                                       padding ) != 0 )
        {
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        }

        rec->type = MBEDTLS_SSL_MSG_APPLICATION_DATA;
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    /*
     * Add CID information
     */
    rec->cid_len = transform->out_cid_len;
    memcpy( rec->cid, transform->out_cid, transform->out_cid_len );
    MBEDTLS_SSL_DEBUG_BUF( 3, "CID", rec->cid, rec->cid_len );

    if( rec->cid_len != 0 )
    {
        size_t padding =
            ssl_compute_padding_length( rec->data_len,
                                        MBEDTLS_SSL_CID_PADDING_GRANULARITY );
        /*
         * Wrap plaintext into DTLSInnerPlaintext structure.
         * See ssl_build_inner_plaintext() for more information.
         *
         * Note that this changes `rec->data_len`, and hence
         * `post_avail` needs to be recalculated afterwards.
         */
        if( ssl_build_inner_plaintext( data,
                        &rec->data_len,
                        post_avail,
                        rec->type,
                        padding ) != 0 )
        {
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        }

        rec->type = MBEDTLS_SSL_MSG_CID;
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    post_avail = rec->buf_len - ( rec->data_len + rec->data_offset );

    /*
     * Add MAC before if needed
     */
#if defined(MBEDTLS_SSL_SOME_MODES_USE_MAC)
    if( mode == MBEDTLS_MODE_STREAM ||
        ( mode == MBEDTLS_MODE_CBC
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
          && transform->encrypt_then_mac == MBEDTLS_SSL_ETM_DISABLED
#endif
        ) )
    {
        if( post_avail < transform->maclen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "Buffer provided for encrypted record not large enough" ) );
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        }

#if defined(MBEDTLS_SSL_PROTO_SSL3)
        if( transform->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            unsigned char mac[SSL3_MAC_MAX_BYTES];
            int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
            ret = ssl_mac( &transform->md_ctx_enc, transform->mac_enc,
                           data, rec->data_len, rec->ctr, rec->type, mac );
            if( ret == 0 )
                memcpy( data + rec->data_len, mac, transform->maclen );
            mbedtls_platform_zeroize( mac, transform->maclen );
            if( ret != 0 )
            {
                MBEDTLS_SSL_DEBUG_RET( 1, "ssl_mac", ret );
                return( ret );
            }
        }
        else
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
        defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if( transform->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_1 )
        {
            unsigned char mac[MBEDTLS_SSL_MAC_ADD];
            int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

            ssl_extract_add_data_from_record( add_data, &add_data_len, rec,
                                              transform->minor_ver );

            ret = mbedtls_md_hmac_update( &transform->md_ctx_enc,
                                          add_data, add_data_len );
            if( ret != 0 )
                goto hmac_failed_etm_disabled;
            ret = mbedtls_md_hmac_update( &transform->md_ctx_enc,
                                          data, rec->data_len );
            if( ret != 0 )
                goto hmac_failed_etm_disabled;
            ret = mbedtls_md_hmac_finish( &transform->md_ctx_enc, mac );
            if( ret != 0 )
                goto hmac_failed_etm_disabled;
            ret = mbedtls_md_hmac_reset( &transform->md_ctx_enc );
            if( ret != 0 )
                goto hmac_failed_etm_disabled;

            memcpy( data + rec->data_len, mac, transform->maclen );

        hmac_failed_etm_disabled:
            mbedtls_platform_zeroize( mac, transform->maclen );
            if( ret != 0 )
            {
                MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_md_hmac_xxx", ret );
                return( ret );
            }
        }
        else
#endif
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

        MBEDTLS_SSL_DEBUG_BUF( 4, "computed mac", data + rec->data_len,
                               transform->maclen );

        rec->data_len += transform->maclen;
        post_avail -= transform->maclen;
        auth_done++;
    }
#endif /* MBEDTLS_SSL_SOME_MODES_USE_MAC */

    /*
     * Encrypt
     */
#if defined(MBEDTLS_ARC4_C) || defined(MBEDTLS_CIPHER_NULL_CIPHER)
    if( mode == MBEDTLS_MODE_STREAM )
    {
        int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
        size_t olen;
        MBEDTLS_SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %" MBEDTLS_PRINTF_SIZET ", "
                                    "including %d bytes of padding",
                                    rec->data_len, 0 ) );

        if( ( ret = mbedtls_cipher_crypt( &transform->cipher_ctx_enc,
                                   transform->iv_enc, transform->ivlen,
                                   data, rec->data_len,
                                   data, &olen ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_crypt", ret );
            return( ret );
        }

        if( rec->data_len != olen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }
    }
    else
#endif /* MBEDTLS_ARC4_C || MBEDTLS_CIPHER_NULL_CIPHER */

#if defined(MBEDTLS_GCM_C) || \
    defined(MBEDTLS_CCM_C) || \
    defined(MBEDTLS_CHACHAPOLY_C)
    if( mode == MBEDTLS_MODE_GCM ||
        mode == MBEDTLS_MODE_CCM ||
        mode == MBEDTLS_MODE_CHACHAPOLY )
    {
        int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
        unsigned char iv[12];
        unsigned char *dynamic_iv;
        size_t dynamic_iv_len;
        int dynamic_iv_is_explicit =
            ssl_transform_aead_dynamic_iv_is_explicit( transform );

        /* Check that there's space for the authentication tag. */
        if( post_avail < transform->taglen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "Buffer provided for encrypted record not large enough" ) );
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        }

        /*
         * Build nonce for AEAD encryption.
         *
         * Note: In the case of CCM and GCM in TLS 1.2, the dynamic
         *       part of the IV is prepended to the ciphertext and
         *       can be chosen freely - in particular, it need not
         *       agree with the record sequence number.
         *       However, since ChaChaPoly as well as all AEAD modes
         *       in TLS 1.3 use the record sequence number as the
         *       dynamic part of the nonce, we uniformly use the
         *       record sequence number here in all cases.
         */
        dynamic_iv     = rec->ctr;
        dynamic_iv_len = sizeof( rec->ctr );

        ssl_build_record_nonce( iv, sizeof( iv ),
                                transform->iv_enc,
                                transform->fixed_ivlen,
                                dynamic_iv,
                                dynamic_iv_len );

        /*
         * Build additional data for AEAD encryption.
         * This depends on the TLS version.
         */
        ssl_extract_add_data_from_record( add_data, &add_data_len, rec,
                                          transform->minor_ver );

        MBEDTLS_SSL_DEBUG_BUF( 4, "IV used (internal)",
                               iv, transform->ivlen );
        MBEDTLS_SSL_DEBUG_BUF( 4, "IV used (transmitted)",
                               dynamic_iv,
                               dynamic_iv_is_explicit ? dynamic_iv_len : 0 );
        MBEDTLS_SSL_DEBUG_BUF( 4, "additional data used for AEAD",
                               add_data, add_data_len );
        MBEDTLS_SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %" MBEDTLS_PRINTF_SIZET ", "
                                    "including 0 bytes of padding",
                                    rec->data_len ) );

        /*
         * Encrypt and authenticate
         */

        if( ( ret = mbedtls_cipher_auth_encrypt_ext( &transform->cipher_ctx_enc,
                   iv, transform->ivlen,
                   add_data, add_data_len,
                   data, rec->data_len,                     /* src */
                   data, rec->buf_len - (data - rec->buf),  /* dst */
                   &rec->data_len,
                   transform->taglen ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_auth_encrypt", ret );
            return( ret );
        }
        MBEDTLS_SSL_DEBUG_BUF( 4, "after encrypt: tag",
                               data + rec->data_len - transform->taglen,
                               transform->taglen );
        /* Account for authentication tag. */
        post_avail -= transform->taglen;

        /*
         * Prefix record content with dynamic IV in case it is explicit.
         */
        if( dynamic_iv_is_explicit != 0 )
        {
            if( rec->data_offset < dynamic_iv_len )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "Buffer provided for encrypted record not large enough" ) );
                return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
            }

            memcpy( data - dynamic_iv_len, dynamic_iv, dynamic_iv_len );
            rec->data_offset -= dynamic_iv_len;
            rec->data_len    += dynamic_iv_len;
        }

        auth_done++;
    }
    else
#endif /* MBEDTLS_GCM_C || MBEDTLS_CCM_C || MBEDTLS_CHACHAPOLY_C */
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC)
    if( mode == MBEDTLS_MODE_CBC )
    {
        int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
        size_t padlen, i;
        size_t olen;

        /* Currently we're always using minimal padding
         * (up to 255 bytes would be allowed). */
        padlen = transform->ivlen - ( rec->data_len + 1 ) % transform->ivlen;
        if( padlen == transform->ivlen )
            padlen = 0;

        /* Check there's enough space in the buffer for the padding. */
        if( post_avail < padlen + 1 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "Buffer provided for encrypted record not large enough" ) );
            return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
        }

        for( i = 0; i <= padlen; i++ )
            data[rec->data_len + i] = (unsigned char) padlen;

        rec->data_len += padlen + 1;
        post_avail -= padlen + 1;

#if defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2)
        /*
         * Prepend per-record IV for block cipher in TLS v1.1 and up as per
         * Method 1 (6.2.3.2. in RFC4346 and RFC5246)
         */
        if( transform->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            if( f_rng == NULL )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "No PRNG provided to encrypt_record routine" ) );
                return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
            }

            if( rec->data_offset < transform->ivlen )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "Buffer provided for encrypted record not large enough" ) );
                return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
            }

            /*
             * Generate IV
             */
            ret = f_rng( p_rng, transform->iv_enc, transform->ivlen );
            if( ret != 0 )
                return( ret );

            memcpy( data - transform->ivlen, transform->iv_enc,
                    transform->ivlen );

        }
#endif /* MBEDTLS_SSL_PROTO_TLS1_1 || MBEDTLS_SSL_PROTO_TLS1_2 */

        MBEDTLS_SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %" MBEDTLS_PRINTF_SIZET ", "
                            "including %" MBEDTLS_PRINTF_SIZET
                            " bytes of IV and %" MBEDTLS_PRINTF_SIZET " bytes of padding",
                            rec->data_len, transform->ivlen,
                            padlen + 1 ) );

        if( ( ret = mbedtls_cipher_crypt( &transform->cipher_ctx_enc,
                                   transform->iv_enc,
                                   transform->ivlen,
                                   data, rec->data_len,
                                   data, &olen ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_crypt", ret );
            return( ret );
        }

        if( rec->data_len != olen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1)
        if( transform->minor_ver < MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            /*
             * Save IV in SSL3 and TLS1
             */
            memcpy( transform->iv_enc, transform->cipher_ctx_enc.iv,
                    transform->ivlen );
        }
        else
#endif
        {
            data             -= transform->ivlen;
            rec->data_offset -= transform->ivlen;
            rec->data_len    += transform->ivlen;
        }

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
        if( auth_done == 0 )
        {
            unsigned char mac[MBEDTLS_SSL_MAC_ADD];

            /*
             * MAC(MAC_write_key, seq_num +
             *     TLSCipherText.type +
             *     TLSCipherText.version +
             *     length_of( (IV +) ENC(...) ) +
             *     IV + // except for TLS 1.0
             *     ENC(content + padding + padding_length));
             */

            if( post_avail < transform->maclen)
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "Buffer provided for encrypted record not large enough" ) );
                return( MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL );
            }

            ssl_extract_add_data_from_record( add_data, &add_data_len,
                                              rec, transform->minor_ver );

            MBEDTLS_SSL_DEBUG_MSG( 3, ( "using encrypt then mac" ) );
            MBEDTLS_SSL_DEBUG_BUF( 4, "MAC'd meta-data", add_data,
                                   add_data_len );

            ret = mbedtls_md_hmac_update( &transform->md_ctx_enc, add_data,
                                          add_data_len );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;
            ret = mbedtls_md_hmac_update( &transform->md_ctx_enc,
                                          data, rec->data_len );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;
            ret = mbedtls_md_hmac_finish( &transform->md_ctx_enc, mac );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;
            ret = mbedtls_md_hmac_reset( &transform->md_ctx_enc );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;

            memcpy( data + rec->data_len, mac, transform->maclen );

            rec->data_len += transform->maclen;
            post_avail -= transform->maclen;
            auth_done++;

        hmac_failed_etm_enabled:
            mbedtls_platform_zeroize( mac, transform->maclen );
            if( ret != 0 )
            {
                MBEDTLS_SSL_DEBUG_RET( 1, "HMAC calculation failed", ret );
                return( ret );
            }
        }
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */
    }
    else
#endif /* MBEDTLS_SSL_SOME_SUITES_USE_CBC) */
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    /* Make extra sure authentication was performed, exactly once */
    if( auth_done != 1 )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= encrypt buf" ) );

    return( 0 );
}