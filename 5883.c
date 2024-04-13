static int ssl_buffer_future_record( mbedtls_ssl_context *ssl,
                                     mbedtls_record const *rec )
{
    mbedtls_ssl_handshake_params * const hs = ssl->handshake;

    /* Don't buffer future records outside handshakes. */
    if( hs == NULL )
        return( 0 );

    /* Only buffer handshake records (we are only interested
     * in Finished messages). */
    if( rec->type != MBEDTLS_SSL_MSG_HANDSHAKE )
        return( 0 );

    /* Don't buffer more than one future epoch record. */
    if( hs->buffering.future_record.data != NULL )
        return( 0 );

    /* Don't buffer record if there's not enough buffering space remaining. */
    if( rec->buf_len > ( MBEDTLS_SSL_DTLS_MAX_BUFFERING -
                         hs->buffering.total_bytes_buffered ) )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Buffering of future epoch record of size %" MBEDTLS_PRINTF_SIZET
                                    " would exceed the compile-time limit %" MBEDTLS_PRINTF_SIZET
                                    " (already %" MBEDTLS_PRINTF_SIZET
                                    " bytes buffered) -- ignore\n",
                        rec->buf_len, (size_t) MBEDTLS_SSL_DTLS_MAX_BUFFERING,
                        hs->buffering.total_bytes_buffered ) );
        return( 0 );
    }

    /* Buffer record */
    MBEDTLS_SSL_DEBUG_MSG( 2, ( "Buffer record from epoch %u",
                                ssl->in_epoch + 1U ) );
    MBEDTLS_SSL_DEBUG_BUF( 3, "Buffered record", rec->buf, rec->buf_len );

    /* ssl_parse_record_header() only considers records
     * of the next epoch as candidates for buffering. */
    hs->buffering.future_record.epoch = ssl->in_epoch + 1;
    hs->buffering.future_record.len   = rec->buf_len;

    hs->buffering.future_record.data =
        mbedtls_calloc( 1, hs->buffering.future_record.len );
    if( hs->buffering.future_record.data == NULL )
    {
        /* If we run out of RAM trying to buffer a
         * record from the next epoch, just ignore. */
        return( 0 );
    }

    memcpy( hs->buffering.future_record.data, rec->buf, rec->buf_len );

    hs->buffering.total_bytes_buffered += rec->buf_len;
    return( 0 );
}