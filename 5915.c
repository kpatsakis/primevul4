static int ssl_load_buffered_message( mbedtls_ssl_context *ssl )
{
    mbedtls_ssl_handshake_params * const hs = ssl->handshake;
    mbedtls_ssl_hs_buffer * hs_buf;
    int ret = 0;

    if( hs == NULL )
        return( -1 );

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> ssl_load_buffered_messsage" ) );

    if( ssl->state == MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC ||
        ssl->state == MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC )
    {
        /* Check if we have seen a ChangeCipherSpec before.
         * If yes, synthesize a CCS record. */
        if( !hs->buffering.seen_ccs )
        {
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "CCS not seen in the current flight" ) );
            ret = -1;
            goto exit;
        }

        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Injecting buffered CCS message" ) );
        ssl->in_msgtype = MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC;
        ssl->in_msglen = 1;
        ssl->in_msg[0] = 1;

        /* As long as they are equal, the exact value doesn't matter. */
        ssl->in_left            = 0;
        ssl->next_record_offset = 0;

        hs->buffering.seen_ccs = 0;
        goto exit;
    }

#if defined(MBEDTLS_DEBUG_C)
    /* Debug only */
    {
        unsigned offset;
        for( offset = 1; offset < MBEDTLS_SSL_MAX_BUFFERED_HS; offset++ )
        {
            hs_buf = &hs->buffering.hs[offset];
            if( hs_buf->is_valid == 1 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 2, ( "Future message with sequence number %u %s buffered.",
                            hs->in_msg_seq + offset,
                            hs_buf->is_complete ? "fully" : "partially" ) );
            }
        }
    }
#endif /* MBEDTLS_DEBUG_C */

    /* Check if we have buffered and/or fully reassembled the
     * next handshake message. */
    hs_buf = &hs->buffering.hs[0];
    if( ( hs_buf->is_valid == 1 ) && ( hs_buf->is_complete == 1 ) )
    {
        /* Synthesize a record containing the buffered HS message. */
        size_t msg_len = ( hs_buf->data[1] << 16 ) |
                         ( hs_buf->data[2] << 8  ) |
                           hs_buf->data[3];

        /* Double-check that we haven't accidentally buffered
         * a message that doesn't fit into the input buffer. */
        if( msg_len + 12 > MBEDTLS_SSL_IN_CONTENT_LEN )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
        }

        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Next handshake message has been buffered - load" ) );
        MBEDTLS_SSL_DEBUG_BUF( 3, "Buffered handshake message (incl. header)",
                               hs_buf->data, msg_len + 12 );

        ssl->in_msgtype = MBEDTLS_SSL_MSG_HANDSHAKE;
        ssl->in_hslen   = msg_len + 12;
        ssl->in_msglen  = msg_len + 12;
        memcpy( ssl->in_msg, hs_buf->data, ssl->in_hslen );

        ret = 0;
        goto exit;
    }
    else
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Next handshake message %u not or only partially bufffered",
                                    hs->in_msg_seq ) );
    }

    ret = -1;

exit:

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= ssl_load_buffered_message" ) );
    return( ret );
}