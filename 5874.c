int mbedtls_ssl_flight_transmit( mbedtls_ssl_context *ssl )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> mbedtls_ssl_flight_transmit" ) );

    if( ssl->handshake->retransmit_state != MBEDTLS_SSL_RETRANS_SENDING )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "initialise flight transmission" ) );

        ssl->handshake->cur_msg = ssl->handshake->flight;
        ssl->handshake->cur_msg_p = ssl->handshake->flight->p + 12;
        ret = ssl_swap_epochs( ssl );
        if( ret != 0 )
            return( ret );

        ssl->handshake->retransmit_state = MBEDTLS_SSL_RETRANS_SENDING;
    }

    while( ssl->handshake->cur_msg != NULL )
    {
        size_t max_frag_len;
        const mbedtls_ssl_flight_item * const cur = ssl->handshake->cur_msg;

        int const is_finished =
            ( cur->type == MBEDTLS_SSL_MSG_HANDSHAKE &&
              cur->p[0] == MBEDTLS_SSL_HS_FINISHED );

        uint8_t const force_flush = ssl->disable_datagram_packing == 1 ?
            SSL_FORCE_FLUSH : SSL_DONT_FORCE_FLUSH;

        /* Swap epochs before sending Finished: we can't do it after
         * sending ChangeCipherSpec, in case write returns WANT_READ.
         * Must be done before copying, may change out_msg pointer */
        if( is_finished && ssl->handshake->cur_msg_p == ( cur->p + 12 ) )
        {
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "swap epochs to send finished message" ) );
            ret = ssl_swap_epochs( ssl );
            if( ret != 0 )
                return( ret );
        }

        ret = ssl_get_remaining_payload_in_datagram( ssl );
        if( ret < 0 )
            return( ret );
        max_frag_len = (size_t) ret;

        /* CCS is copied as is, while HS messages may need fragmentation */
        if( cur->type == MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC )
        {
            if( max_frag_len == 0 )
            {
                if( ( ret = mbedtls_ssl_flush_output( ssl ) ) != 0 )
                    return( ret );

                continue;
            }

            memcpy( ssl->out_msg, cur->p, cur->len );
            ssl->out_msglen  = cur->len;
            ssl->out_msgtype = cur->type;

            /* Update position inside current message */
            ssl->handshake->cur_msg_p += cur->len;
        }
        else
        {
            const unsigned char * const p = ssl->handshake->cur_msg_p;
            const size_t hs_len = cur->len - 12;
            const size_t frag_off = p - ( cur->p + 12 );
            const size_t rem_len = hs_len - frag_off;
            size_t cur_hs_frag_len, max_hs_frag_len;

            if( ( max_frag_len < 12 ) || ( max_frag_len == 12 && hs_len != 0 ) )
            {
                if( is_finished )
                {
                    ret = ssl_swap_epochs( ssl );
                    if( ret != 0 )
                        return( ret );
                }

                if( ( ret = mbedtls_ssl_flush_output( ssl ) ) != 0 )
                    return( ret );

                continue;
            }
            max_hs_frag_len = max_frag_len - 12;

            cur_hs_frag_len = rem_len > max_hs_frag_len ?
                max_hs_frag_len : rem_len;

            if( frag_off == 0 && cur_hs_frag_len != hs_len )
            {
                MBEDTLS_SSL_DEBUG_MSG( 2, ( "fragmenting handshake message (%u > %u)",
                                            (unsigned) cur_hs_frag_len,
                                            (unsigned) max_hs_frag_len ) );
            }

            /* Messages are stored with handshake headers as if not fragmented,
             * copy beginning of headers then fill fragmentation fields.
             * Handshake headers: type(1) len(3) seq(2) f_off(3) f_len(3) */
            memcpy( ssl->out_msg, cur->p, 6 );

            ssl->out_msg[6] = MBEDTLS_BYTE_2( frag_off );
            ssl->out_msg[7] = MBEDTLS_BYTE_1( frag_off );
            ssl->out_msg[8] = MBEDTLS_BYTE_0( frag_off );

            ssl->out_msg[ 9] = MBEDTLS_BYTE_2( cur_hs_frag_len );
            ssl->out_msg[10] = MBEDTLS_BYTE_1( cur_hs_frag_len );
            ssl->out_msg[11] = MBEDTLS_BYTE_0( cur_hs_frag_len );

            MBEDTLS_SSL_DEBUG_BUF( 3, "handshake header", ssl->out_msg, 12 );

            /* Copy the handshake message content and set records fields */
            memcpy( ssl->out_msg + 12, p, cur_hs_frag_len );
            ssl->out_msglen = cur_hs_frag_len + 12;
            ssl->out_msgtype = cur->type;

            /* Update position inside current message */
            ssl->handshake->cur_msg_p += cur_hs_frag_len;
        }

        /* If done with the current message move to the next one if any */
        if( ssl->handshake->cur_msg_p >= cur->p + cur->len )
        {
            if( cur->next != NULL )
            {
                ssl->handshake->cur_msg = cur->next;
                ssl->handshake->cur_msg_p = cur->next->p + 12;
            }
            else
            {
                ssl->handshake->cur_msg = NULL;
                ssl->handshake->cur_msg_p = NULL;
            }
        }

        /* Actually send the message out */
        if( ( ret = mbedtls_ssl_write_record( ssl, force_flush ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_write_record", ret );
            return( ret );
        }
    }

    if( ( ret = mbedtls_ssl_flush_output( ssl ) ) != 0 )
        return( ret );

    /* Update state and set timer */
    if( ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER )
        ssl->handshake->retransmit_state = MBEDTLS_SSL_RETRANS_FINISHED;
    else
    {
        ssl->handshake->retransmit_state = MBEDTLS_SSL_RETRANS_WAITING;
        mbedtls_ssl_set_timer( ssl, ssl->handshake->retransmit_timeout );
    }

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= mbedtls_ssl_flight_transmit" ) );

    return( 0 );
}