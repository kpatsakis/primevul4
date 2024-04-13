static void ssl_extract_add_data_from_record( unsigned char* add_data,
                                              size_t *add_data_len,
                                              mbedtls_record *rec,
                                              unsigned minor_ver )
{
    /* Quoting RFC 5246 (TLS 1.2):
     *
     *    additional_data = seq_num + TLSCompressed.type +
     *                      TLSCompressed.version + TLSCompressed.length;
     *
     * For the CID extension, this is extended as follows
     * (quoting draft-ietf-tls-dtls-connection-id-05,
     *  https://tools.ietf.org/html/draft-ietf-tls-dtls-connection-id-05):
     *
     *       additional_data = seq_num + DTLSPlaintext.type +
     *                         DTLSPlaintext.version +
     *                         cid +
     *                         cid_length +
     *                         length_of_DTLSInnerPlaintext;
     *
     * For TLS 1.3, the record sequence number is dropped from the AAD
     * and encoded within the nonce of the AEAD operation instead.
     */

    unsigned char *cur = add_data;

#if defined(MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL)
    if( minor_ver != MBEDTLS_SSL_MINOR_VERSION_4 )
#endif /* MBEDTLS_SSL_PROTO_TLS1_3_EXPERIMENTAL */
    {
        ((void) minor_ver);
        memcpy( cur, rec->ctr, sizeof( rec->ctr ) );
        cur += sizeof( rec->ctr );
    }

    *cur = rec->type;
    cur++;

    memcpy( cur, rec->ver, sizeof( rec->ver ) );
    cur += sizeof( rec->ver );

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    if( rec->cid_len != 0 )
    {
        memcpy( cur, rec->cid, rec->cid_len );
        cur += rec->cid_len;

        *cur = rec->cid_len;
        cur++;

        MBEDTLS_PUT_UINT16_BE( rec->data_len, cur, 0 );
        cur += 2;
    }
    else
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
    {
        MBEDTLS_PUT_UINT16_BE( rec->data_len, cur, 0 );
        cur += 2;
    }

    *add_data_len = cur - add_data;
}