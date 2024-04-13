static int ssl_buffer_make_space( mbedtls_ssl_context *ssl,
                                  size_t desired )
{
    int offset;
    mbedtls_ssl_handshake_params * const hs = ssl->handshake;
    MBEDTLS_SSL_DEBUG_MSG( 2, ( "Attempt to free buffered messages to have %u bytes available",
                                (unsigned) desired ) );

    /* Get rid of future records epoch first, if such exist. */
    ssl_free_buffered_record( ssl );

    /* Check if we have enough space available now. */
    if( desired <= ( MBEDTLS_SSL_DTLS_MAX_BUFFERING -
                     hs->buffering.total_bytes_buffered ) )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Enough space available after freeing future epoch record" ) );
        return( 0 );
    }

    /* We don't have enough space to buffer the next expected handshake
     * message. Remove buffers used for future messages to gain space,
     * starting with the most distant one. */
    for( offset = MBEDTLS_SSL_MAX_BUFFERED_HS - 1;
         offset >= 0; offset-- )
    {
        MBEDTLS_SSL_DEBUG_MSG( 2, ( "Free buffering slot %d to make space for reassembly of next handshake message",
                                    offset ) );

        ssl_buffering_free_slot( ssl, (uint8_t) offset );

        /* Check if we have enough space available now. */
        if( desired <= ( MBEDTLS_SSL_DTLS_MAX_BUFFERING -
                         hs->buffering.total_bytes_buffered ) )
        {
            MBEDTLS_SSL_DEBUG_MSG( 2, ( "Enough space available after freeing buffered HS messages" ) );
            return( 0 );
        }
    }

    return( -1 );
}