static int ssl_prepare_record_content( mbedtls_ssl_context *ssl,
                                       mbedtls_record *rec )
{
    int ret, done = 0;

    MBEDTLS_SSL_DEBUG_BUF( 4, "input record from network",
                           rec->buf, rec->buf_len );

#if defined(MBEDTLS_SSL_HW_RECORD_ACCEL)
    if( mbedtls_ssl_hw_record_read != NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "going for mbedtls_ssl_hw_record_read()" ) );

        ret = mbedtls_ssl_hw_record_read( ssl );
        if( ret != 0 && ret != MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_hw_record_read", ret );
            return( MBEDTLS_ERR_SSL_HW_ACCEL_FAILED );
        }

        if( ret == 0 )
            done = 1;
    }
#endif /* MBEDTLS_SSL_HW_RECORD_ACCEL */
    if( !done && ssl->transform_in != NULL )
    {
        unsigned char const old_msg_type = rec->type;

        if( ( ret = mbedtls_ssl_decrypt_buf( ssl, ssl->transform_in,
                                             rec ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_decrypt_buf", ret );

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
            if( ret == MBEDTLS_ERR_SSL_UNEXPECTED_CID &&
                ssl->conf->ignore_unexpected_cid
                    == MBEDTLS_SSL_UNEXPECTED_CID_IGNORE )
            {
                MBEDTLS_SSL_DEBUG_MSG( 3, ( "ignoring unexpected CID" ) );
                ret = MBEDTLS_ERR_SSL_CONTINUE_PROCESSING;
            }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

            return( ret );
        }

        if( old_msg_type != rec->type )
        {
            MBEDTLS_SSL_DEBUG_MSG( 4, ( "record type after decrypt (before %d): %d",
                                        old_msg_type, rec->type ) );
        }

        MBEDTLS_SSL_DEBUG_BUF( 4, "input payload after decrypt",
                               rec->buf + rec->data_offset, rec->data_len );

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        /* We have already checked the record content type
         * in ssl_parse_record_header(), failing or silently
         * dropping the record in the case of an unknown type.
         *
         * Since with the use of CIDs, the record content type
         * might change during decryption, re-check the record
         * content type, but treat a failure as fatal this time. */
        if( ssl_check_record_type( rec->type ) )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "unknown record type" ) );
            return( MBEDTLS_ERR_SSL_INVALID_RECORD );
        }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

        if( rec->data_len == 0 )
        {
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
            if( ssl->minor_ver == MBEDTLS_SSL_MINOR_VERSION_3
                && rec->type != MBEDTLS_SSL_MSG_APPLICATION_DATA )
            {
                /* TLS v1.2 explicitly disallows zero-length messages which are not application data */
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "invalid zero-length message type: %d", ssl->in_msgtype ) );
                return( MBEDTLS_ERR_SSL_INVALID_RECORD );
            }
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

            ssl->nb_zero++;

            /*
             * Three or more empty messages may be a DoS attack
             * (excessive CPU consumption).
             */
            if( ssl->nb_zero > 3 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "received four consecutive empty "
                                            "messages, possible DoS attack" ) );
                /* Treat the records as if they were not properly authenticated,
                 * thereby failing the connection if we see more than allowed
                 * by the configured bad MAC threshold. */
                return( MBEDTLS_ERR_SSL_INVALID_MAC );
            }
        }
        else
            ssl->nb_zero = 0;

#if defined(MBEDTLS_SSL_PROTO_DTLS)
        if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        {
            ; /* in_ctr read from peer, not maintained internally */
        }
        else
#endif
        {
            unsigned i;
            for( i = 8; i > mbedtls_ssl_ep_len( ssl ); i-- )
                if( ++ssl->in_ctr[i - 1] != 0 )
                    break;

            /* The loop goes to its end iff the counter is wrapping */
            if( i == mbedtls_ssl_ep_len( ssl ) )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "incoming message counter would wrap" ) );
                return( MBEDTLS_ERR_SSL_COUNTER_WRAPPING );
            }
        }

    }

#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        mbedtls_ssl_dtls_replay_update( ssl );
    }
#endif

    /* Check actual (decrypted) record content length against
     * configured maximum. */
    if( ssl->in_msglen > MBEDTLS_SSL_IN_CONTENT_LEN )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad message length" ) );
        return( MBEDTLS_ERR_SSL_INVALID_RECORD );
    }

    return( 0 );
}