void mbedtls_ssl_reset_in_out_pointers( mbedtls_ssl_context *ssl )
{
    /* Set the incoming and outgoing record pointers. */
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        ssl->out_hdr = ssl->out_buf;
        ssl->in_hdr  = ssl->in_buf;
    }
    else
#endif /* MBEDTLS_SSL_PROTO_DTLS */
    {
        ssl->out_hdr = ssl->out_buf + 8;
        ssl->in_hdr  = ssl->in_buf  + 8;
    }

    /* Derive other internal pointers. */
    mbedtls_ssl_update_out_pointers( ssl, NULL /* no transform enabled */ );
    mbedtls_ssl_update_in_pointers ( ssl );
}