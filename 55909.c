static int x509_crt_check_parent( const mbedtls_x509_crt *child,
                                  const mbedtls_x509_crt *parent,
                                  int top, int bottom )
{
    int need_ca_bit;

    /* Parent must be the issuer */
    if( x509_name_cmp( &child->issuer, &parent->subject ) != 0 )
        return( -1 );

    /* Parent must have the basicConstraints CA bit set as a general rule */
    need_ca_bit = 1;

    /* Exception: v1/v2 certificates that are locally trusted. */
    if( top && parent->version < 3 )
        need_ca_bit = 0;

    /* Exception: self-signed end-entity certs that are locally trusted. */
    if( top && bottom &&
        child->raw.len == parent->raw.len &&
        memcmp( child->raw.p, parent->raw.p, child->raw.len ) == 0 )
    {
        need_ca_bit = 0;
    }

    if( need_ca_bit && ! parent->ca_istrue )
        return( -1 );

#if defined(MBEDTLS_X509_CHECK_KEY_USAGE)
    if( need_ca_bit &&
        mbedtls_x509_crt_check_key_usage( parent, MBEDTLS_X509_KU_KEY_CERT_SIGN ) != 0 )
    {
        return( -1 );
    }
#endif

    return( 0 );
}
