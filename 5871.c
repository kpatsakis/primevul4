int mbedtls_ssl_decrypt_buf( mbedtls_ssl_context const *ssl,
                             mbedtls_ssl_transform *transform,
                             mbedtls_record *rec )
{
    size_t olen;
    mbedtls_cipher_mode_t mode;
    int ret, auth_done = 0;
#if defined(MBEDTLS_SSL_SOME_MODES_USE_MAC)
    size_t padlen = 0, correct = 1;
#endif
    unsigned char* data;
    unsigned char add_data[13 + 1 + MBEDTLS_SSL_CID_IN_LEN_MAX ];
    size_t add_data_len;

#if !defined(MBEDTLS_DEBUG_C)
    ssl = NULL; /* make sure we don't use it except for debug */
    ((void) ssl);
#endif

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> decrypt buf" ) );
    if( rec == NULL                     ||
        rec->buf == NULL                ||
        rec->buf_len < rec->data_offset ||
        rec->buf_len - rec->data_offset < rec->data_len )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad record structure provided to decrypt_buf" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    data = rec->buf + rec->data_offset;
    mode = mbedtls_cipher_get_cipher_mode( &transform->cipher_ctx_dec );

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    /*
     * Match record's CID with incoming CID.
     */
    if( rec->cid_len != transform->in_cid_len ||
        memcmp( rec->cid, transform->in_cid, rec->cid_len ) != 0 )
    {
        return( MBEDTLS_ERR_SSL_UNEXPECTED_CID );
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_ARC4_C) || defined(MBEDTLS_CIPHER_NULL_CIPHER)
    if( mode == MBEDTLS_MODE_STREAM )
    {
        padlen = 0;
        if( ( ret = mbedtls_cipher_crypt( &transform->cipher_ctx_dec,
                                   transform->iv_dec,
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
        unsigned char iv[12];
        unsigned char *dynamic_iv;
        size_t dynamic_iv_len;

        /*
         * Extract dynamic part of nonce for AEAD decryption.
         *
         * Note: In the case of CCM and GCM in TLS 1.2, the dynamic
         *       part of the IV is prepended to the ciphertext and
         *       can be chosen freely - in particular, it need not
         *       agree with the record sequence number.
         */
        dynamic_iv_len = sizeof( rec->ctr );
        if( ssl_transform_aead_dynamic_iv_is_explicit( transform ) == 1 )
        {
            if( rec->data_len < dynamic_iv_len )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "msglen (%" MBEDTLS_PRINTF_SIZET
                                            " ) < explicit_iv_len (%" MBEDTLS_PRINTF_SIZET ") ",
                                            rec->data_len,
                                            dynamic_iv_len ) );
                return( MBEDTLS_ERR_SSL_INVALID_MAC );
            }
            dynamic_iv = data;

            data += dynamic_iv_len;
            rec->data_offset += dynamic_iv_len;
            rec->data_len    -= dynamic_iv_len;
        }
        else
        {
            dynamic_iv = rec->ctr;
        }

        /* Check that there's space for the authentication tag. */
        if( rec->data_len < transform->taglen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "msglen (%" MBEDTLS_PRINTF_SIZET
                                        ") < taglen (%" MBEDTLS_PRINTF_SIZET ") ",
                                        rec->data_len,
                                        transform->taglen ) );
            return( MBEDTLS_ERR_SSL_INVALID_MAC );
        }
        rec->data_len -= transform->taglen;

        /*
         * Prepare nonce from dynamic and static parts.
         */
        ssl_build_record_nonce( iv, sizeof( iv ),
                                transform->iv_dec,
                                transform->fixed_ivlen,
                                dynamic_iv,
                                dynamic_iv_len );

        /*
         * Build additional data for AEAD encryption.
         * This depends on the TLS version.
         */
        ssl_extract_add_data_from_record( add_data, &add_data_len, rec,
                                          transform->minor_ver );
        MBEDTLS_SSL_DEBUG_BUF( 4, "additional data used for AEAD",
                               add_data, add_data_len );

        /* Because of the check above, we know that there are
         * explicit_iv_len Bytes preceding data, and taglen
         * bytes following data + data_len. This justifies
         * the debug message and the invocation of
         * mbedtls_cipher_auth_decrypt() below. */

        MBEDTLS_SSL_DEBUG_BUF( 4, "IV used", iv, transform->ivlen );
        MBEDTLS_SSL_DEBUG_BUF( 4, "TAG used", data + rec->data_len,
                               transform->taglen );

        /*
         * Decrypt and authenticate
         */
        if( ( ret = mbedtls_cipher_auth_decrypt_ext( &transform->cipher_ctx_dec,
                  iv, transform->ivlen,
                  add_data, add_data_len,
                  data, rec->data_len + transform->taglen,          /* src */
                  data, rec->buf_len - (data - rec->buf), &olen,    /* dst */
                  transform->taglen ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_auth_decrypt", ret );

            if( ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED )
                return( MBEDTLS_ERR_SSL_INVALID_MAC );

            return( ret );
        }
        auth_done++;

        /* Double-check that AEAD decryption doesn't change content length. */
        if( olen != rec->data_len )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }
    }
    else
#endif /* MBEDTLS_GCM_C || MBEDTLS_CCM_C */
#if defined(MBEDTLS_SSL_SOME_SUITES_USE_CBC)
    if( mode == MBEDTLS_MODE_CBC )
    {
        size_t minlen = 0;

        /*
         * Check immediate ciphertext sanity
         */
#if defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if( transform->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            /* The ciphertext is prefixed with the CBC IV. */
            minlen += transform->ivlen;
        }
#endif

        /* Size considerations:
         *
         * - The CBC cipher text must not be empty and hence
         *   at least of size transform->ivlen.
         *
         * Together with the potential IV-prefix, this explains
         * the first of the two checks below.
         *
         * - The record must contain a MAC, either in plain or
         *   encrypted, depending on whether Encrypt-then-MAC
         *   is used or not.
         *   - If it is, the message contains the IV-prefix,
         *     the CBC ciphertext, and the MAC.
         *   - If it is not, the padded plaintext, and hence
         *     the CBC ciphertext, has at least length maclen + 1
         *     because there is at least the padding length byte.
         *
         * As the CBC ciphertext is not empty, both cases give the
         * lower bound minlen + maclen + 1 on the record size, which
         * we test for in the second check below.
         */
        if( rec->data_len < minlen + transform->ivlen ||
            rec->data_len < minlen + transform->maclen + 1 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "msglen (%" MBEDTLS_PRINTF_SIZET
                                        ") < max( ivlen(%" MBEDTLS_PRINTF_SIZET
                                        "), maclen (%" MBEDTLS_PRINTF_SIZET ") "
                                "+ 1 ) ( + expl IV )", rec->data_len,
                                transform->ivlen,
                                transform->maclen ) );
            return( MBEDTLS_ERR_SSL_INVALID_MAC );
        }

        /*
         * Authenticate before decrypt if enabled
         */
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
        if( transform->encrypt_then_mac == MBEDTLS_SSL_ETM_ENABLED )
        {
            unsigned char mac_expect[MBEDTLS_SSL_MAC_ADD];

            MBEDTLS_SSL_DEBUG_MSG( 3, ( "using encrypt then mac" ) );

            /* Update data_len in tandem with add_data.
             *
             * The subtraction is safe because of the previous check
             * data_len >= minlen + maclen + 1.
             *
             * Afterwards, we know that data + data_len is followed by at
             * least maclen Bytes, which justifies the call to
             * mbedtls_ct_memcmp() below.
             *
             * Further, we still know that data_len > minlen */
            rec->data_len -= transform->maclen;
            ssl_extract_add_data_from_record( add_data, &add_data_len, rec,
                                              transform->minor_ver );

            /* Calculate expected MAC. */
            MBEDTLS_SSL_DEBUG_BUF( 4, "MAC'd meta-data", add_data,
                                   add_data_len );
            ret = mbedtls_md_hmac_update( &transform->md_ctx_dec, add_data,
                                          add_data_len );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;
            ret = mbedtls_md_hmac_update( &transform->md_ctx_dec,
                                    data, rec->data_len );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;
            ret = mbedtls_md_hmac_finish( &transform->md_ctx_dec, mac_expect );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;
            ret = mbedtls_md_hmac_reset( &transform->md_ctx_dec );
            if( ret != 0 )
                goto hmac_failed_etm_enabled;

            MBEDTLS_SSL_DEBUG_BUF( 4, "message  mac", data + rec->data_len,
                                   transform->maclen );
            MBEDTLS_SSL_DEBUG_BUF( 4, "expected mac", mac_expect,
                                   transform->maclen );

            /* Compare expected MAC with MAC at the end of the record. */
            if( mbedtls_ct_memcmp( data + rec->data_len, mac_expect,
                                              transform->maclen ) != 0 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "message mac does not match" ) );
                ret = MBEDTLS_ERR_SSL_INVALID_MAC;
                goto hmac_failed_etm_enabled;
            }
            auth_done++;

        hmac_failed_etm_enabled:
            mbedtls_platform_zeroize( mac_expect, transform->maclen );
            if( ret != 0 )
            {
                if( ret != MBEDTLS_ERR_SSL_INVALID_MAC )
                    MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_hmac_xxx", ret );
                return( ret );
            }
        }
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */

        /*
         * Check length sanity
         */

        /* We know from above that data_len > minlen >= 0,
         * so the following check in particular implies that
         * data_len >= minlen + ivlen ( = minlen or 2 * minlen ). */
        if( rec->data_len % transform->ivlen != 0 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "msglen (%" MBEDTLS_PRINTF_SIZET
                                        ") %% ivlen (%" MBEDTLS_PRINTF_SIZET ") != 0",
                                        rec->data_len, transform->ivlen ) );
            return( MBEDTLS_ERR_SSL_INVALID_MAC );
        }

#if defined(MBEDTLS_SSL_PROTO_TLS1_1) || defined(MBEDTLS_SSL_PROTO_TLS1_2)
        /*
         * Initialize for prepended IV for block cipher in TLS v1.1 and up
         */
        if( transform->minor_ver >= MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            /* Safe because data_len >= minlen + ivlen = 2 * ivlen. */
            memcpy( transform->iv_dec, data, transform->ivlen );

            data += transform->ivlen;
            rec->data_offset += transform->ivlen;
            rec->data_len -= transform->ivlen;
        }
#endif /* MBEDTLS_SSL_PROTO_TLS1_1 || MBEDTLS_SSL_PROTO_TLS1_2 */

        /* We still have data_len % ivlen == 0 and data_len >= ivlen here. */

        if( ( ret = mbedtls_cipher_crypt( &transform->cipher_ctx_dec,
                                   transform->iv_dec, transform->ivlen,
                                   data, rec->data_len, data, &olen ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_cipher_crypt", ret );
            return( ret );
        }

        /* Double-check that length hasn't changed during decryption. */
        if( rec->data_len != olen )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1)
        if( transform->minor_ver < MBEDTLS_SSL_MINOR_VERSION_2 )
        {
            /*
             * Save IV in SSL3 and TLS1, where CBC decryption of consecutive
             * records is equivalent to CBC decryption of the concatenation
             * of the records; in other words, IVs are maintained across
             * record decryptions.
             */
            memcpy( transform->iv_dec, transform->cipher_ctx_dec.iv,
                    transform->ivlen );
        }
#endif

        /* Safe since data_len >= minlen + maclen + 1, so after having
         * subtracted at most minlen and maclen up to this point,
         * data_len > 0 (because of data_len % ivlen == 0, it's actually
         * >= ivlen ). */
        padlen = data[rec->data_len - 1];

        if( auth_done == 1 )
        {
            const size_t mask = mbedtls_ct_size_mask_ge(
                                rec->data_len,
                                padlen + 1 );
            correct &= mask;
            padlen  &= mask;
        }
        else
        {
#if defined(MBEDTLS_SSL_DEBUG_ALL)
            if( rec->data_len < transform->maclen + padlen + 1 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "msglen (%" MBEDTLS_PRINTF_SIZET
                                            ") < maclen (%" MBEDTLS_PRINTF_SIZET
                                            ") + padlen (%" MBEDTLS_PRINTF_SIZET ")",
                                            rec->data_len,
                                            transform->maclen,
                                            padlen + 1 ) );
            }
#endif

            const size_t mask = mbedtls_ct_size_mask_ge(
                                rec->data_len,
                                transform->maclen + padlen + 1 );
            correct &= mask;
            padlen  &= mask;
        }

        padlen++;

        /* Regardless of the validity of the padding,
         * we have data_len >= padlen here. */

#if defined(MBEDTLS_SSL_PROTO_SSL3)
        if( transform->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            /* This is the SSL 3.0 path, we don't have to worry about Lucky
             * 13, because there's a strictly worse padding attack built in
             * the protocol (known as part of POODLE), so we don't care if the
             * code is not constant-time, in particular branches are OK. */
            if( padlen > transform->ivlen )
            {
#if defined(MBEDTLS_SSL_DEBUG_ALL)
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad padding length: is %" MBEDTLS_PRINTF_SIZET ", "
                                            "should be no more than %" MBEDTLS_PRINTF_SIZET,
                                            padlen, transform->ivlen ) );
#endif
                correct = 0;
            }
        }
        else
#endif /* MBEDTLS_SSL_PROTO_SSL3 */
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if( transform->minor_ver > MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            /* The padding check involves a series of up to 256
             * consecutive memory reads at the end of the record
             * plaintext buffer. In order to hide the length and
             * validity of the padding, always perform exactly
             * `min(256,plaintext_len)` reads (but take into account
             * only the last `padlen` bytes for the padding check). */
            size_t pad_count = 0;
            volatile unsigned char* const check = data;

            /* Index of first padding byte; it has been ensured above
             * that the subtraction is safe. */
            size_t const padding_idx = rec->data_len - padlen;
            size_t const num_checks = rec->data_len <= 256 ? rec->data_len : 256;
            size_t const start_idx = rec->data_len - num_checks;
            size_t idx;

            for( idx = start_idx; idx < rec->data_len; idx++ )
            {
                /* pad_count += (idx >= padding_idx) &&
                 *              (check[idx] == padlen - 1);
                 */
                const size_t mask = mbedtls_ct_size_mask_ge( idx, padding_idx );
                const size_t equal = mbedtls_ct_size_bool_eq( check[idx],
                                                              padlen - 1 );
                pad_count += mask & equal;
            }
            correct &= mbedtls_ct_size_bool_eq( pad_count, padlen );

#if defined(MBEDTLS_SSL_DEBUG_ALL)
            if( padlen > 0 && correct == 0 )
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad padding byte detected" ) );
#endif
            padlen &= mbedtls_ct_size_mask( correct );
        }
        else
#endif /* MBEDTLS_SSL_PROTO_TLS1 || MBEDTLS_SSL_PROTO_TLS1_1 || \
          MBEDTLS_SSL_PROTO_TLS1_2 */
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

        /* If the padding was found to be invalid, padlen == 0
         * and the subtraction is safe. If the padding was found valid,
         * padlen hasn't been changed and the previous assertion
         * data_len >= padlen still holds. */
        rec->data_len -= padlen;
    }
    else
#endif /* MBEDTLS_SSL_SOME_SUITES_USE_CBC */
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

#if defined(MBEDTLS_SSL_DEBUG_ALL)
    MBEDTLS_SSL_DEBUG_BUF( 4, "raw buffer after decryption",
                           data, rec->data_len );
#endif

    /*
     * Authenticate if not done yet.
     * Compute the MAC regardless of the padding result (RFC4346, CBCTIME).
     */
#if defined(MBEDTLS_SSL_SOME_MODES_USE_MAC)
    if( auth_done == 0 )
    {
        unsigned char mac_expect[MBEDTLS_SSL_MAC_ADD] = { 0 };
        unsigned char mac_peer[MBEDTLS_SSL_MAC_ADD] = { 0 };

        /* If the initial value of padlen was such that
         * data_len < maclen + padlen + 1, then padlen
         * got reset to 1, and the initial check
         * data_len >= minlen + maclen + 1
         * guarantees that at this point we still
         * have at least data_len >= maclen.
         *
         * If the initial value of padlen was such that
         * data_len >= maclen + padlen + 1, then we have
         * subtracted either padlen + 1 (if the padding was correct)
         * or 0 (if the padding was incorrect) since then,
         * hence data_len >= maclen in any case.
         */
        rec->data_len -= transform->maclen;
        ssl_extract_add_data_from_record( add_data, &add_data_len, rec,
                                          transform->minor_ver );

#if defined(MBEDTLS_SSL_PROTO_SSL3)
        if( transform->minor_ver == MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            ret = ssl_mac( &transform->md_ctx_dec,
                           transform->mac_dec,
                           data, rec->data_len,
                           rec->ctr, rec->type,
                           mac_expect );
            if( ret != 0 )
            {
                MBEDTLS_SSL_DEBUG_RET( 1, "ssl_mac", ret );
                goto hmac_failed_etm_disabled;
            }
            memcpy( mac_peer, data + rec->data_len, transform->maclen );
        }
        else
#endif /* MBEDTLS_SSL_PROTO_SSL3 */
#if defined(MBEDTLS_SSL_PROTO_TLS1) || defined(MBEDTLS_SSL_PROTO_TLS1_1) || \
        defined(MBEDTLS_SSL_PROTO_TLS1_2)
        if( transform->minor_ver > MBEDTLS_SSL_MINOR_VERSION_0 )
        {
            /*
             * The next two sizes are the minimum and maximum values of
             * data_len over all padlen values.
             *
             * They're independent of padlen, since we previously did
             * data_len -= padlen.
             *
             * Note that max_len + maclen is never more than the buffer
             * length, as we previously did in_msglen -= maclen too.
             */
            const size_t max_len = rec->data_len + padlen;
            const size_t min_len = ( max_len > 256 ) ? max_len - 256 : 0;

            ret = mbedtls_ct_hmac( &transform->md_ctx_dec,
                                   add_data, add_data_len,
                                   data, rec->data_len, min_len, max_len,
                                   mac_expect );
            if( ret != 0 )
            {
                MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ct_hmac", ret );
                goto hmac_failed_etm_disabled;
            }

            mbedtls_ct_memcpy_offset( mac_peer, data,
                                      rec->data_len,
                                      min_len, max_len,
                                      transform->maclen );
        }
        else
#endif /* MBEDTLS_SSL_PROTO_TLS1 || MBEDTLS_SSL_PROTO_TLS1_1 || \
              MBEDTLS_SSL_PROTO_TLS1_2 */
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

#if defined(MBEDTLS_SSL_DEBUG_ALL)
        MBEDTLS_SSL_DEBUG_BUF( 4, "expected mac", mac_expect, transform->maclen );
        MBEDTLS_SSL_DEBUG_BUF( 4, "message  mac", mac_peer, transform->maclen );
#endif

        if( mbedtls_ct_memcmp( mac_peer, mac_expect,
                                          transform->maclen ) != 0 )
        {
#if defined(MBEDTLS_SSL_DEBUG_ALL)
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "message mac does not match" ) );
#endif
            correct = 0;
        }
        auth_done++;

    hmac_failed_etm_disabled:
        mbedtls_platform_zeroize( mac_peer, transform->maclen );
        mbedtls_platform_zeroize( mac_expect, transform->maclen );
        if( ret != 0 )
            return( ret );
    }

    /*
     * Finally check the correct flag
     */
    if( correct == 0 )
        return( MBEDTLS_ERR_SSL_INVALID_MAC );
#endif /* MBEDTLS_SSL_SOME_MODES_USE_MAC */

    /* Make extra sure authentication was performed, exactly once */
    if( auth_done != 1 )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    if( transform->minor_ver == MBEDTLS_SSL_MINOR_VERSION_4 )
    {
        /* Remove inner padding and infer true content type. */
        ret = ssl_parse_inner_plaintext( data, &rec->data_len,
                                         &rec->type );

        if( ret != 0 )
            return( MBEDTLS_ERR_SSL_INVALID_RECORD );
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    if( rec->cid_len != 0 )
    {
        ret = ssl_parse_inner_plaintext( data, &rec->data_len,
                                         &rec->type );
        if( ret != 0 )
            return( MBEDTLS_ERR_SSL_INVALID_RECORD );
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= decrypt buf" ) );

    return( 0 );
}