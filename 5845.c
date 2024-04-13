void mbedtls_ssl_update_out_pointers( mbedtls_ssl_context *ssl,
                                      mbedtls_ssl_transform *transform )
{
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        ssl->out_ctr = ssl->out_hdr +  3;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        ssl->out_cid = ssl->out_ctr +  8;
        ssl->out_len = ssl->out_cid;
        if( transform != NULL )
            ssl->out_len += transform->out_cid_len;
#else /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
        ssl->out_len = ssl->out_ctr + 8;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
        ssl->out_iv  = ssl->out_len + 2;
    }
    else
#endif
    {
        ssl->out_ctr = ssl->out_hdr - 8;
        ssl->out_len = ssl->out_hdr + 3;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        ssl->out_cid = ssl->out_len;
#endif
        ssl->out_iv  = ssl->out_hdr + 5;
    }

    ssl->out_msg = ssl->out_iv;
    /* Adjust out_msg to make space for explicit IV, if used. */
    if( transform != NULL )
        ssl->out_msg += ssl_transform_get_explicit_iv_len( transform );
}