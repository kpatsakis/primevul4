void mbedtls_ssl_buffering_free( mbedtls_ssl_context *ssl )
{
    unsigned offset;
    mbedtls_ssl_handshake_params * const hs = ssl->handshake;

    if( hs == NULL )
        return;

    ssl_free_buffered_record( ssl );

    for( offset = 0; offset < MBEDTLS_SSL_MAX_BUFFERED_HS; offset++ )
        ssl_buffering_free_slot( ssl, offset );
}