int mbedtls_ssl_prepare_handshake_record( mbedtls_ssl_context *ssl )
{
    if( ssl->in_msglen < mbedtls_ssl_hs_hdr_len( ssl ) )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "handshake message too short: %" MBEDTLS_PRINTF_SIZET,
                            ssl->in_msglen ) );
        return( MBEDTLS_ERR_SSL_INVALID_RECORD );
    }

    ssl->in_hslen = mbedtls_ssl_hs_hdr_len( ssl ) + ssl_get_hs_total_len( ssl );

    MBEDTLS_SSL_DEBUG_MSG( 3, ( "handshake message: msglen ="
                        " %" MBEDTLS_PRINTF_SIZET ", type = %u, hslen = %" MBEDTLS_PRINTF_SIZET,
                        ssl->in_msglen, ssl->in_msg[0], ssl->in_hslen ) );

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
        unsigned int recv_msg_seq = ( ssl->in_msg[4] << 8 ) | ssl->in_msg[5];

        if( ssl_check_hs_header( ssl ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "invalid handshake header" ) );
            return( MBEDTLS_ERR_SSL_INVALID_RECORD );
        }

        if( ssl->handshake != NULL &&
            ( ( ssl->state   != MBEDTLS_SSL_HANDSHAKE_OVER &&
                recv_msg_seq != ssl->handshake->in_msg_seq ) ||
              ( ssl->state  == MBEDTLS_SSL_HANDSHAKE_OVER &&
                ssl->in_msg[0] != MBEDTLS_SSL_HS_CLIENT_HELLO ) ) )
        {
            if( recv_msg_seq > ssl->handshake->in_msg_seq )
            {
                MBEDTLS_SSL_DEBUG_MSG( 2, ( "received future handshake message of sequence number %u (next %u)",
                                            recv_msg_seq,
                                            ssl->handshake->in_msg_seq ) );
                return( MBEDTLS_ERR_SSL_EARLY_MESSAGE );
            }

            /* Retransmit only on last message from previous flight, to avoid
             * too many retransmissions.
             * Besides, No sane server ever retransmits HelloVerifyRequest */
            if( recv_msg_seq == ssl->handshake->in_flight_start_seq - 1 &&
                ssl->in_msg[0] != MBEDTLS_SSL_HS_HELLO_VERIFY_REQUEST )
            {
                MBEDTLS_SSL_DEBUG_MSG( 2, ( "received message from last flight, "
                                    "message_seq = %u, start_of_flight = %u",
                                    recv_msg_seq,
                                    ssl->handshake->in_flight_start_seq ) );

                if( ( ret = mbedtls_ssl_resend( ssl ) ) != 0 )
                {
                    MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_resend", ret );
                    return( ret );
                }
            }
            else
            {
                MBEDTLS_SSL_DEBUG_MSG( 2, ( "dropping out-of-sequence message: "
                                    "message_seq = %u, expected = %u",
                                    recv_msg_seq,
                                    ssl->handshake->in_msg_seq ) );
            }

            return( MBEDTLS_ERR_SSL_CONTINUE_PROCESSING );
        }
        /* Wait until message completion to increment in_msg_seq */

        /* Message reassembly is handled alongside buffering of future
         * messages; the commonality is that both handshake fragments and
         * future messages cannot be forwarded immediately to the
         * handshake logic layer. */
        if( ssl_hs_is_proper_fragment( ssl ) == 1 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "found fragmented DTLS handshake message" ) );
            return( MBEDTLS_ERR_SSL_EARLY_MESSAGE );
        }
    }
    else
#endif /* MBEDTLS_SSL_PROTO_DTLS */
    /* With TLS we don't handle fragmentation (for now) */
    if( ssl->in_msglen < ssl->in_hslen )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "TLS handshake fragmentation not supported" ) );
        return( MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE );
    }

    return( 0 );
}