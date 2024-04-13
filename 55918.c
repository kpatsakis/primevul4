static int x509_get_uid( unsigned char **p,
                         const unsigned char *end,
                         mbedtls_x509_buf *uid, int n )
{
    int ret;

    if( *p == end )
        return( 0 );

    uid->tag = **p;

    if( ( ret = mbedtls_asn1_get_tag( p, end, &uid->len,
            MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_ASN1_CONSTRUCTED | n ) ) != 0 )
    {
        if( ret == MBEDTLS_ERR_ASN1_UNEXPECTED_TAG )
            return( 0 );

        return( ret );
    }

    uid->p = *p;
    *p += uid->len;

    return( 0 );
}
