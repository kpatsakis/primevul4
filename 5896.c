static int ssl_buffer_message( mbedtls_ssl_context *ssl )
{
    int ret = 0;
    mbedtls_ssl_handshake_params * const hs = ssl->handshake;

    if( hs == NULL )
        return( 0 );

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> ssl_buffer_message" ) );

    switch( ssl->in_msgtype )
    {
        case MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC:
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "Remember CCS message" ) );

            hs->buffering.seen_ccs = 1;
            break;

        case MBEDTLS_SSL_MSG_HANDSHAKE:
        {
            unsigned recv_msg_seq_offset;
            unsigned recv_msg_seq = ( ssl->in_msg[4] << 8 ) | ssl->in_msg[5];
            mbedtls_ssl_hs_buffer *hs_buf;
            size_t msg_len = ssl->in_hslen - 12;

            /* We should never receive an old handshake
             * message - double-check nonetheless. */
            if( recv_msg_seq < ssl->handshake->in_msg_seq )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
                return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
            }

            recv_msg_seq_offset = recv_msg_seq - ssl->handshake->in_msg_seq;
            if( recv_msg_seq_offset >= MBEDTLS_SSL_MAX_BUFFERED_HS )
            {
                /* Silently ignore -- message too far in the future */
                MBEDTLS_SSL_DEBUG_MSG( 2,
                 ( "Ignore future HS message with sequence number %u, "
                   "buffering window %u - %u",
                   recv_msg_seq, ssl->handshake->in_msg_seq,
                   ssl->handshake->in_msg_seq + MBEDTLS_SSL_MAX_BUFFERED_HS - 1 ) );

                goto exit;
            }

            MBEDTLS_SSL_DEBUG_MSG( 2, ( "Buffering HS message with sequence number %u, offset %u ",
                                        recv_msg_seq, recv_msg_seq_offset ) );

            hs_buf = &hs->buffering.hs[ recv_msg_seq_offset ];

            /* Check if the buffering for this seq nr has already commenced. */
            if( !hs_buf->is_valid )
            {
                size_t reassembly_buf_sz;

                hs_buf->is_fragmented =
                    ( ssl_hs_is_proper_fragment( ssl ) == 1 );

                /* We copy the message back into the input buffer
                 * after reassembly, so check that it's not too large.
                 * This is an implementation-specific limitation
                 * and not one from the standard, hence it is not
                 * checked in ssl_check_hs_header(). */
                if( msg_len + 12 > MBEDTLS_SSL_IN_CONTENT_LEN )
                {
                    /* Ignore message */
                    goto exit;
                }

                /* Check if we have enough space to buffer the message. */
                if( hs->buffering.total_bytes_buffered >
                    MBEDTLS_SSL_DTLS_MAX_BUFFERING )
                {
                    MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
                    return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
                }

                reassembly_buf_sz = ssl_get_reassembly_buffer_size( msg_len,
                                                       hs_buf->is_fragmented );

                if( reassembly_buf_sz > ( MBEDTLS_SSL_DTLS_MAX_BUFFERING -
                                          hs->buffering.total_bytes_buffered ) )
                {
                    if( recv_msg_seq_offset > 0 )
                    {
                        /* If we can't buffer a future message because
                         * of space limitations -- ignore. */
                        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Buffering of future message of size %" MBEDTLS_PRINTF_SIZET
                                                    " would exceed the compile-time limit %" MBEDTLS_PRINTF_SIZET
                                                    " (already %" MBEDTLS_PRINTF_SIZET
                                                    " bytes buffered) -- ignore\n",
                             msg_len, (size_t) MBEDTLS_SSL_DTLS_MAX_BUFFERING,
                             hs->buffering.total_bytes_buffered ) );
                        goto exit;
                    }
                    else
                    {
                        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Buffering of future message of size %" MBEDTLS_PRINTF_SIZET
                                                    " would exceed the compile-time limit %" MBEDTLS_PRINTF_SIZET
                                                    " (already %" MBEDTLS_PRINTF_SIZET
                                                    " bytes buffered) -- attempt to make space by freeing buffered future messages\n",
                             msg_len, (size_t) MBEDTLS_SSL_DTLS_MAX_BUFFERING,
                             hs->buffering.total_bytes_buffered ) );
                    }

                    if( ssl_buffer_make_space( ssl, reassembly_buf_sz ) != 0 )
                    {
                        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Reassembly of next message of size %" MBEDTLS_PRINTF_SIZET
                                                    " (%" MBEDTLS_PRINTF_SIZET " with bitmap) would exceed"
                                                    " the compile-time limit %" MBEDTLS_PRINTF_SIZET
                                                    " (already %" MBEDTLS_PRINTF_SIZET
                                                    " bytes buffered) -- fail\n",
                             msg_len,
                             reassembly_buf_sz,
                             (size_t) MBEDTLS_SSL_DTLS_MAX_BUFFERING,
                             hs->buffering.total_bytes_buffered ) );
                        ret = MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL;
                        goto exit;
                    }
                }

                MBEDTLS_SSL_DEBUG_MSG( 2, ( "initialize reassembly, total length = %" MBEDTLS_PRINTF_SIZET,
                                            msg_len ) );

                hs_buf->data = mbedtls_calloc( 1, reassembly_buf_sz );
                if( hs_buf->data == NULL )
                {
                    ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    goto exit;
                }
                hs_buf->data_len = reassembly_buf_sz;

                /* Prepare final header: copy msg_type, length and message_seq,
                 * then add standardised fragment_offset and fragment_length */
                memcpy( hs_buf->data, ssl->in_msg, 6 );
                memset( hs_buf->data + 6, 0, 3 );
                memcpy( hs_buf->data + 9, hs_buf->data + 1, 3 );

                hs_buf->is_valid = 1;

                hs->buffering.total_bytes_buffered += reassembly_buf_sz;
            }
            else
            {
                /* Make sure msg_type and length are consistent */
                if( memcmp( hs_buf->data, ssl->in_msg, 4 ) != 0 )
                {
                    MBEDTLS_SSL_DEBUG_MSG( 1, ( "Fragment header mismatch - ignore" ) );
                    /* Ignore */
                    goto exit;
                }
            }

            if( !hs_buf->is_complete )
            {
                size_t frag_len, frag_off;
                unsigned char * const msg = hs_buf->data + 12;

                /*
                 * Check and copy current fragment
                 */

                /* Validation of header fields already done in
                 * mbedtls_ssl_prepare_handshake_record(). */
                frag_off = ssl_get_hs_frag_off( ssl );
                frag_len = ssl_get_hs_frag_len( ssl );

                MBEDTLS_SSL_DEBUG_MSG( 2, ( "adding fragment, offset = %" MBEDTLS_PRINTF_SIZET
                                            ", length = %" MBEDTLS_PRINTF_SIZET,
                                            frag_off, frag_len ) );
                memcpy( msg + frag_off, ssl->in_msg + 12, frag_len );

                if( hs_buf->is_fragmented )
                {
                    unsigned char * const bitmask = msg + msg_len;
                    ssl_bitmask_set( bitmask, frag_off, frag_len );
                    hs_buf->is_complete = ( ssl_bitmask_check( bitmask,
                                                               msg_len ) == 0 );
                }
                else
                {
                    hs_buf->is_complete = 1;
                }

                MBEDTLS_SSL_DEBUG_MSG( 2, ( "message %scomplete",
                                   hs_buf->is_complete ? "" : "not yet " ) );
            }

            break;
        }

        default:
            /* We don't buffer other types of messages. */
            break;
    }

exit:

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= ssl_buffer_message" ) );
    return( ret );
}