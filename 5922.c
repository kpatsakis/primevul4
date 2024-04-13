static int ssl_get_next_record( mbedtls_ssl_context *ssl )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_record rec;

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    /* We might have buffered a future record; if so,
     * and if the epoch matches now, load it.
     * On success, this call will set ssl->in_left to
     * the length of the buffered record, so that
     * the calls to ssl_fetch_input() below will
     * essentially be no-ops. */
    ret = ssl_load_buffered_record( ssl );
    if( ret != 0 )
        return( ret );
#endif /* MBEDTLS_SSL_PROTO_DTLS */

    /* Ensure that we have enough space available for the default form
     * of TLS / DTLS record headers (5 Bytes for TLS, 13 Bytes for DTLS,
     * with no space for CIDs counted in). */
    ret = mbedtls_ssl_fetch_input( ssl, mbedtls_ssl_in_hdr_len( ssl ) );
    if( ret != 0 )
    {
        MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_fetch_input", ret );
        return( ret );
    }

    ret = ssl_parse_record_header( ssl, ssl->in_hdr, ssl->in_left, &rec );
    if( ret != 0 )
    {
#if defined(MBEDTLS_SSL_PROTO_DTLS)
        if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        {
            if( ret == MBEDTLS_ERR_SSL_EARLY_MESSAGE )
            {
                ret = ssl_buffer_future_record( ssl, &rec );
                if( ret != 0 )
                    return( ret );

                /* Fall through to handling of unexpected records */
                ret = MBEDTLS_ERR_SSL_UNEXPECTED_RECORD;
            }

            if( ret == MBEDTLS_ERR_SSL_UNEXPECTED_RECORD )
            {
#if defined(MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE) && defined(MBEDTLS_SSL_SRV_C)
                /* Reset in pointers to default state for TLS/DTLS records,
                 * assuming no CID and no offset between record content and
                 * record plaintext. */
                mbedtls_ssl_update_in_pointers( ssl );

                /* Setup internal message pointers from record structure. */
                ssl->in_msgtype = rec.type;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
                ssl->in_len = ssl->in_cid + rec.cid_len;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
                ssl->in_iv  = ssl->in_msg = ssl->in_len + 2;
                ssl->in_msglen = rec.data_len;

                ret = ssl_check_client_reconnect( ssl );
                MBEDTLS_SSL_DEBUG_RET( 2, "ssl_check_client_reconnect", ret );
                if( ret != 0 )
                    return( ret );
#endif

                /* Skip unexpected record (but not whole datagram) */
                ssl->next_record_offset = rec.buf_len;

                MBEDTLS_SSL_DEBUG_MSG( 1, ( "discarding unexpected record "
                                            "(header)" ) );
            }
            else
            {
                /* Skip invalid record and the rest of the datagram */
                ssl->next_record_offset = 0;
                ssl->in_left = 0;

                MBEDTLS_SSL_DEBUG_MSG( 1, ( "discarding invalid record "
                                            "(header)" ) );
            }

            /* Get next record */
            return( MBEDTLS_ERR_SSL_CONTINUE_PROCESSING );
        }
        else
#endif
        {
            return( ret );
        }
    }

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        /* Remember offset of next record within datagram. */
        ssl->next_record_offset = rec.buf_len;
        if( ssl->next_record_offset < ssl->in_left )
        {
            MBEDTLS_SSL_DEBUG_MSG( 3, ( "more than one record within datagram" ) );
        }
    }
    else
#endif
    {
        /*
         * Fetch record contents from underlying transport.
         */
        ret = mbedtls_ssl_fetch_input( ssl, rec.buf_len );
        if( ret != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_fetch_input", ret );
            return( ret );
        }

        ssl->in_left = 0;
    }

    /*
     * Decrypt record contents.
     */

    if( ( ret = ssl_prepare_record_content( ssl, &rec ) ) != 0 )
    {
#if defined(MBEDTLS_SSL_PROTO_DTLS)
        if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        {
            /* Silently discard invalid records */
            if( ret == MBEDTLS_ERR_SSL_INVALID_MAC )
            {
                /* Except when waiting for Finished as a bad mac here
                 * probably means something went wrong in the handshake
                 * (eg wrong psk used, mitm downgrade attempt, etc.) */
                if( ssl->state == MBEDTLS_SSL_CLIENT_FINISHED ||
                    ssl->state == MBEDTLS_SSL_SERVER_FINISHED )
                {
#if defined(MBEDTLS_SSL_ALL_ALERT_MESSAGES)
                    if( ret == MBEDTLS_ERR_SSL_INVALID_MAC )
                    {
                        mbedtls_ssl_send_alert_message( ssl,
                                MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                MBEDTLS_SSL_ALERT_MSG_BAD_RECORD_MAC );
                    }
#endif
                    return( ret );
                }

#if defined(MBEDTLS_SSL_DTLS_BADMAC_LIMIT)
                if( ssl->conf->badmac_limit != 0 &&
                    ++ssl->badmac_seen >= ssl->conf->badmac_limit )
                {
                    MBEDTLS_SSL_DEBUG_MSG( 1, ( "too many records with bad MAC" ) );
                    return( MBEDTLS_ERR_SSL_INVALID_MAC );
                }
#endif

                /* As above, invalid records cause
                 * dismissal of the whole datagram. */

                ssl->next_record_offset = 0;
                ssl->in_left = 0;

                MBEDTLS_SSL_DEBUG_MSG( 1, ( "discarding invalid record (mac)" ) );
                return( MBEDTLS_ERR_SSL_CONTINUE_PROCESSING );
            }

            return( ret );
        }
        else
#endif
        {
            /* Error out (and send alert) on invalid records */
#if defined(MBEDTLS_SSL_ALL_ALERT_MESSAGES)
            if( ret == MBEDTLS_ERR_SSL_INVALID_MAC )
            {
                mbedtls_ssl_send_alert_message( ssl,
                        MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                        MBEDTLS_SSL_ALERT_MSG_BAD_RECORD_MAC );
            }
#endif
            return( ret );
        }
    }


    /* Reset in pointers to default state for TLS/DTLS records,
     * assuming no CID and no offset between record content and
     * record plaintext. */
    mbedtls_ssl_update_in_pointers( ssl );
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    ssl->in_len = ssl->in_cid + rec.cid_len;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
    ssl->in_iv  = ssl->in_len + 2;

    /* The record content type may change during decryption,
     * so re-read it. */
    ssl->in_msgtype = rec.type;
    /* Also update the input buffer, because unfortunately
     * the server-side ssl_parse_client_hello() reparses the
     * record header when receiving a ClientHello initiating
     * a renegotiation. */
    ssl->in_hdr[0] = rec.type;
    ssl->in_msg    = rec.buf + rec.data_offset;
    ssl->in_msglen = rec.data_len;
    MBEDTLS_PUT_UINT16_BE( rec.data_len, ssl->in_len, 0 );

#if defined(MBEDTLS_ZLIB_SUPPORT)
    if( ssl->transform_in != NULL &&
        ssl->session_in->compression == MBEDTLS_SSL_COMPRESS_DEFLATE )
    {
        if( ( ret = ssl_decompress_buf( ssl ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_decompress_buf", ret );
            return( ret );
        }

        /* Check actual (decompress) record content length against
         * configured maximum. */
        if( ssl->in_msglen > MBEDTLS_SSL_IN_CONTENT_LEN )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad message length" ) );
            return( MBEDTLS_ERR_SSL_INVALID_RECORD );
        }
    }
#endif /* MBEDTLS_ZLIB_SUPPORT */

    return( 0 );
}