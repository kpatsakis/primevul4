static int ssl_check_client_reconnect( mbedtls_ssl_context *ssl )
{
    unsigned int rec_epoch = ( ssl->in_ctr[0] << 8 ) | ssl->in_ctr[1];

    /*
     * Check for an epoch 0 ClientHello. We can't use in_msg here to
     * access the first byte of record content (handshake type), as we
     * have an active transform (possibly iv_len != 0), so use the
     * fact that the record header len is 13 instead.
     */
    if( rec_epoch == 0 &&
        ssl->conf->endpoint == MBEDTLS_SSL_IS_SERVER &&
        ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER &&
        ssl->in_msgtype == MBEDTLS_SSL_MSG_HANDSHAKE &&
        ssl->in_left > 13 &&
        ssl->in_buf[13] == MBEDTLS_SSL_HS_CLIENT_HELLO )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "possible client reconnect "
                                    "from the same port" ) );
        return( ssl_handle_possible_reconnect( ssl ) );
    }

    return( 0 );
}