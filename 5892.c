static int ssl_flight_append( mbedtls_ssl_context *ssl )
{
    mbedtls_ssl_flight_item *msg;
    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> ssl_flight_append" ) );
    MBEDTLS_SSL_DEBUG_BUF( 4, "message appended to flight",
                           ssl->out_msg, ssl->out_msglen );

    /* Allocate space for current message */
    if( ( msg = mbedtls_calloc( 1, sizeof(  mbedtls_ssl_flight_item ) ) ) == NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "alloc %" MBEDTLS_PRINTF_SIZET " bytes failed",
                            sizeof( mbedtls_ssl_flight_item ) ) );
        return( MBEDTLS_ERR_SSL_ALLOC_FAILED );
    }

    if( ( msg->p = mbedtls_calloc( 1, ssl->out_msglen ) ) == NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "alloc %" MBEDTLS_PRINTF_SIZET " bytes failed",
                                    ssl->out_msglen ) );
        mbedtls_free( msg );
        return( MBEDTLS_ERR_SSL_ALLOC_FAILED );
    }

    /* Copy current handshake message with headers */
    memcpy( msg->p, ssl->out_msg, ssl->out_msglen );
    msg->len = ssl->out_msglen;
    msg->type = ssl->out_msgtype;
    msg->next = NULL;

    /* Append to the current flight */
    if( ssl->handshake->flight == NULL )
        ssl->handshake->flight = msg;
    else
    {
        mbedtls_ssl_flight_item *cur = ssl->handshake->flight;
        while( cur->next != NULL )
            cur = cur->next;
        cur->next = msg;
    }

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= ssl_flight_append" ) );
    return( 0 );
}