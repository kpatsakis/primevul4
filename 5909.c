int mbedtls_ssl_write_handshake_msg( mbedtls_ssl_context *ssl )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const size_t hs_len = ssl->out_msglen - 4;
    const unsigned char hs_type = ssl->out_msg[0];

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> write handshake message" ) );

    /*
     * Sanity checks
     */
    if( ssl->out_msgtype != MBEDTLS_SSL_MSG_HANDSHAKE          &&
        ssl->out_msgtype != MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC )
    {
        /* In SSLv3, the client might send a NoCertificate alert. */
#if defined(MBEDTLS_SSL_PROTO_SSL3) && defined(MBEDTLS_SSL_CLI_C)
        if( ! ( ssl->minor_ver      == MBEDTLS_SSL_MINOR_VERSION_0 &&
                ssl->out_msgtype    == MBEDTLS_SSL_MSG_ALERT       &&
                ssl->conf->endpoint == MBEDTLS_SSL_IS_CLIENT ) )
#endif /* MBEDTLS_SSL_PROTO_SSL3 && MBEDTLS_SSL_SRV_C */
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }
    }

    /* Whenever we send anything different from a
     * HelloRequest we should be in a handshake - double check. */
    if( ! ( ssl->out_msgtype == MBEDTLS_SSL_MSG_HANDSHAKE &&
            hs_type          == MBEDTLS_SSL_HS_HELLO_REQUEST ) &&
        ssl->handshake == NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM &&
        ssl->handshake != NULL &&
        ssl->handshake->retransmit_state == MBEDTLS_SSL_RETRANS_SENDING )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }
#endif

    /* Double-check that we did not exceed the bounds
     * of the outgoing record buffer.
     * This should never fail as the various message
     * writing functions must obey the bounds of the
     * outgoing record buffer, but better be safe.
     *
     * Note: We deliberately do not check for the MTU or MFL here.
     */
    if( ssl->out_msglen > MBEDTLS_SSL_OUT_CONTENT_LEN )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "Record too large: "
                                    "size %" MBEDTLS_PRINTF_SIZET
                                    ", maximum %" MBEDTLS_PRINTF_SIZET,
                                    ssl->out_msglen,
                                    (size_t) MBEDTLS_SSL_OUT_CONTENT_LEN ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    /*
     * Fill handshake headers
     */
    if( ssl->out_msgtype == MBEDTLS_SSL_MSG_HANDSHAKE )
    {
        ssl->out_msg[1] = MBEDTLS_BYTE_2( hs_len );
        ssl->out_msg[2] = MBEDTLS_BYTE_1( hs_len );
        ssl->out_msg[3] = MBEDTLS_BYTE_0( hs_len );

        /*
         * DTLS has additional fields in the Handshake layer,
         * between the length field and the actual payload:
         *      uint16 message_seq;
         *      uint24 fragment_offset;
         *      uint24 fragment_length;
         */
#if defined(MBEDTLS_SSL_PROTO_DTLS)
        if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        {
            /* Make room for the additional DTLS fields */
            if( MBEDTLS_SSL_OUT_CONTENT_LEN - ssl->out_msglen < 8 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "DTLS handshake message too large: "
                              "size %" MBEDTLS_PRINTF_SIZET ", maximum %" MBEDTLS_PRINTF_SIZET,
                               hs_len,
                               (size_t) ( MBEDTLS_SSL_OUT_CONTENT_LEN - 12 ) ) );
                return( MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
            }

            memmove( ssl->out_msg + 12, ssl->out_msg + 4, hs_len );
            ssl->out_msglen += 8;

            /* Write message_seq and update it, except for HelloRequest */
            if( hs_type != MBEDTLS_SSL_HS_HELLO_REQUEST )
            {
                MBEDTLS_PUT_UINT16_BE( ssl->handshake->out_msg_seq, ssl->out_msg, 4 );
                ++( ssl->handshake->out_msg_seq );
            }
            else
            {
                ssl->out_msg[4] = 0;
                ssl->out_msg[5] = 0;
            }

            /* Handshake hashes are computed without fragmentation,
             * so set frag_offset = 0 and frag_len = hs_len for now */
            memset( ssl->out_msg + 6, 0x00, 3 );
            memcpy( ssl->out_msg + 9, ssl->out_msg + 1, 3 );
        }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

        /* Update running hashes of handshake messages seen */
        if( hs_type != MBEDTLS_SSL_HS_HELLO_REQUEST )
            ssl->handshake->update_checksum( ssl, ssl->out_msg, ssl->out_msglen );
    }

    /* Either send now, or just save to be sent (and resent) later */
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM &&
        ! ( ssl->out_msgtype == MBEDTLS_SSL_MSG_HANDSHAKE &&
            hs_type          == MBEDTLS_SSL_HS_HELLO_REQUEST ) )
    {
        if( ( ret = ssl_flight_append( ssl ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_flight_append", ret );
            return( ret );
        }
    }
    else
#endif
    {
        if( ( ret = mbedtls_ssl_write_record( ssl, SSL_FORCE_FLUSH ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_write_record", ret );
            return( ret );
        }
    }

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= write handshake message" ) );

    return( 0 );
}