int mbedtls_ssl_write_record( mbedtls_ssl_context *ssl, uint8_t force_flush )
{
    int ret, done = 0;
    size_t len = ssl->out_msglen;
    uint8_t flush = force_flush;

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> write record" ) );

#if defined(MBEDTLS_ZLIB_SUPPORT)
    if( ssl->transform_out != NULL &&
        ssl->session_out->compression == MBEDTLS_SSL_COMPRESS_DEFLATE )
    {
        if( ( ret = ssl_compress_buf( ssl ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_compress_buf", ret );
            return( ret );
        }

        len = ssl->out_msglen;
    }
#endif /*MBEDTLS_ZLIB_SUPPORT */

#if defined(MBEDTLS_SSL_HW_RECORD_ACCEL)
    if( mbedtls_ssl_hw_record_write != NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "going for mbedtls_ssl_hw_record_write()" ) );

        ret = mbedtls_ssl_hw_record_write( ssl );
        if( ret != 0 && ret != MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_hw_record_write", ret );
            return( MBEDTLS_ERR_SSL_HW_ACCEL_FAILED );
        }

        if( ret == 0 )
            done = 1;
    }
#endif /* MBEDTLS_SSL_HW_RECORD_ACCEL */
    if( !done )
    {
        unsigned i;
        size_t protected_record_size;
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
        size_t out_buf_len = ssl->out_buf_len;
#else
        size_t out_buf_len = MBEDTLS_SSL_OUT_BUFFER_LEN;
#endif
        /* Skip writing the record content type to after the encryption,
         * as it may change when using the CID extension. */

        mbedtls_ssl_write_version( ssl->major_ver, ssl->minor_ver,
                           ssl->conf->transport, ssl->out_hdr + 1 );

        memcpy( ssl->out_ctr, ssl->cur_out_ctr, 8 );
        MBEDTLS_PUT_UINT16_BE( len, ssl->out_len, 0);

        if( ssl->transform_out != NULL )
        {
            mbedtls_record rec;

            rec.buf         = ssl->out_iv;
            rec.buf_len     = out_buf_len - ( ssl->out_iv - ssl->out_buf );
            rec.data_len    = ssl->out_msglen;
            rec.data_offset = ssl->out_msg - rec.buf;

            memcpy( &rec.ctr[0], ssl->out_ctr, 8 );
            mbedtls_ssl_write_version( ssl->major_ver, ssl->minor_ver,
                                       ssl->conf->transport, rec.ver );
            rec.type = ssl->out_msgtype;

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
            /* The CID is set by mbedtls_ssl_encrypt_buf(). */
            rec.cid_len = 0;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

            if( ( ret = mbedtls_ssl_encrypt_buf( ssl, ssl->transform_out, &rec,
                                         ssl->conf->f_rng, ssl->conf->p_rng ) ) != 0 )
            {
                MBEDTLS_SSL_DEBUG_RET( 1, "ssl_encrypt_buf", ret );
                return( ret );
            }

            if( rec.data_offset != 0 )
            {
                MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
                return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
            }

            /* Update the record content type and CID. */
            ssl->out_msgtype = rec.type;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID )
            memcpy( ssl->out_cid, rec.cid, rec.cid_len );
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
            ssl->out_msglen = len = rec.data_len;
            MBEDTLS_PUT_UINT16_BE( rec.data_len, ssl->out_len, 0 );
        }

        protected_record_size = len + mbedtls_ssl_out_hdr_len( ssl );

#if defined(MBEDTLS_SSL_PROTO_DTLS)
        /* In case of DTLS, double-check that we don't exceed
         * the remaining space in the datagram. */
        if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        {
            ret = ssl_get_remaining_space_in_datagram( ssl );
            if( ret < 0 )
                return( ret );

            if( protected_record_size > (size_t) ret )
            {
                /* Should never happen */
                return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
            }
        }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

        /* Now write the potentially updated record content type. */
        ssl->out_hdr[0] = (unsigned char) ssl->out_msgtype;

        MBEDTLS_SSL_DEBUG_MSG( 3, ( "output record: msgtype = %u, "
                                    "version = [%u:%u], msglen = %" MBEDTLS_PRINTF_SIZET,
                                    ssl->out_hdr[0], ssl->out_hdr[1],
                                    ssl->out_hdr[2], len ) );

        MBEDTLS_SSL_DEBUG_BUF( 4, "output record sent to network",
                               ssl->out_hdr, protected_record_size );

        ssl->out_left += protected_record_size;
        ssl->out_hdr  += protected_record_size;
        mbedtls_ssl_update_out_pointers( ssl, ssl->transform_out );

        for( i = 8; i > mbedtls_ssl_ep_len( ssl ); i-- )
            if( ++ssl->cur_out_ctr[i - 1] != 0 )
                break;

        /* The loop goes to its end iff the counter is wrapping */
        if( i == mbedtls_ssl_ep_len( ssl ) )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "outgoing message counter would wrap" ) );
            return( MBEDTLS_ERR_SSL_COUNTER_WRAPPING );
        }
    }

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM &&
        flush == SSL_DONT_FORCE_FLUSH )
    {
        size_t remaining;
        ret = ssl_get_remaining_payload_in_datagram( ssl );
        if( ret < 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "ssl_get_remaining_payload_in_datagram",
                                   ret );
            return( ret );
        }

        remaining = (size_t) ret;
        if( remaining == 0 )
        {
            flush = SSL_FORCE_FLUSH;
        }
        else
        {
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "Still %u bytes available in current datagram", (unsigned) remaining ) );
        }
    }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

    if( ( flush == SSL_FORCE_FLUSH ) &&
        ( ret = mbedtls_ssl_flush_output( ssl ) ) != 0 )
    {
        MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_flush_output", ret );
        return( ret );
    }

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= write record" ) );

    return( 0 );
}