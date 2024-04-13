static int ssl_get_remaining_payload_in_datagram( mbedtls_ssl_context const *ssl )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t remaining, expansion;
    size_t max_len = MBEDTLS_SSL_OUT_CONTENT_LEN;

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    const size_t mfl = mbedtls_ssl_get_output_max_frag_len( ssl );

    if( max_len > mfl )
        max_len = mfl;

    /* By the standard (RFC 6066 Sect. 4), the MFL extension
     * only limits the maximum record payload size, so in theory
     * we would be allowed to pack multiple records of payload size
     * MFL into a single datagram. However, this would mean that there's
     * no way to explicitly communicate MTU restrictions to the peer.
     *
     * The following reduction of max_len makes sure that we never
     * write datagrams larger than MFL + Record Expansion Overhead.
     */
    if( max_len <= ssl->out_left )
        return( 0 );

    max_len -= ssl->out_left;
#endif

    ret = ssl_get_remaining_space_in_datagram( ssl );
    if( ret < 0 )
        return( ret );
    remaining = (size_t) ret;

    ret = mbedtls_ssl_get_record_expansion( ssl );
    if( ret < 0 )
        return( ret );
    expansion = (size_t) ret;

    if( remaining <= expansion )
        return( 0 );

    remaining -= expansion;
    if( remaining >= max_len )
        remaining = max_len;

    return( (int) remaining );
}