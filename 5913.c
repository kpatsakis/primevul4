static size_t ssl_get_maximum_datagram_size( mbedtls_ssl_context const *ssl )
{
    size_t mtu = mbedtls_ssl_get_current_mtu( ssl );
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    size_t out_buf_len = ssl->out_buf_len;
#else
    size_t out_buf_len = MBEDTLS_SSL_OUT_BUFFER_LEN;
#endif

    if( mtu != 0 && mtu < out_buf_len )
        return( mtu );

    return( out_buf_len );
}