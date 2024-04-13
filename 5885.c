static int ssl_get_remaining_space_in_datagram( mbedtls_ssl_context const *ssl )
{
    size_t const bytes_written = ssl->out_left;
    size_t const mtu           = ssl_get_maximum_datagram_size( ssl );

    /* Double-check that the write-index hasn't gone
     * past what we can transmit in a single datagram. */
    if( bytes_written > mtu )
    {
        /* Should never happen... */
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    return( (int) ( mtu - bytes_written ) );
}