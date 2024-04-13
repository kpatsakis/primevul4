void mbedtls_ssl_update_in_pointers( mbedtls_ssl_context *ssl )
{
    /* This function sets the pointers to match the case
     * of unprotected TLS/DTLS records, with both  ssl->in_iv
     * and ssl->in_msg pointing to the beginning of the record
     * content.
     *
     * When decrypting a protected record, ssl->in_msg
     * will be shifted to point to the beginning of the
     * record plaintext.
     */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
        /* This sets the header pointers to match records
         * without CID. When we receive a record containing
         * a CID, the fields are shifted accordingly in
         * ssl_parse_record_header(). */
        ssl->in_ctr = ssl->in_hdr +  3;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        ssl->in_cid = ssl->in_ctr +  8;
        ssl->in_len = ssl->in_cid; /* Default: no CID */
#else /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
        ssl->in_len = ssl->in_ctr + 8;
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
        ssl->in_iv  = ssl->in_len + 2;
    }
    else
#endif
    {
        ssl->in_ctr = ssl->in_hdr - 8;
        ssl->in_len = ssl->in_hdr + 3;
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        ssl->in_cid = ssl->in_len;
#endif
        ssl->in_iv  = ssl->in_hdr + 5;
    }

    /* This will be adjusted at record decryption time. */
    ssl->in_msg = ssl->in_iv;
}