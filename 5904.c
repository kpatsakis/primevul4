static int ssl_load_buffered_record( mbedtls_ssl_context *ssl )
{
    mbedtls_ssl_handshake_params * const hs = ssl->handshake;
    unsigned char * rec;
    size_t rec_len;
    unsigned rec_epoch;
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    size_t in_buf_len = ssl->in_buf_len;
#else
    size_t in_buf_len = MBEDTLS_SSL_IN_BUFFER_LEN;
#endif
    if( ssl->conf->transport != MBEDTLS_SSL_TRANSPORT_DATAGRAM )
        return( 0 );

    if( hs == NULL )
        return( 0 );

    rec       = hs->buffering.future_record.data;
    rec_len   = hs->buffering.future_record.len;
    rec_epoch = hs->buffering.future_record.epoch;

    if( rec == NULL )
        return( 0 );

    /* Only consider loading future records if the
     * input buffer is empty. */
    if( ssl_next_record_is_in_datagram( ssl ) == 1 )
        return( 0 );

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> ssl_load_buffered_record" ) );

    if( rec_epoch != ssl->in_epoch )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Buffered record not from current epoch." ) );
        goto exit;
    }

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "Found buffered record from current epoch - load" ) );

    /* Double-check that the record is not too large */
    if( rec_len > in_buf_len - (size_t)( ssl->in_hdr - ssl->in_buf ) )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    memcpy( ssl->in_hdr, rec, rec_len );
    ssl->in_left = rec_len;
    ssl->next_record_offset = 0;

    ssl_free_buffered_record( ssl );

exit:
    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= ssl_load_buffered_record" ) );
    return( 0 );
}