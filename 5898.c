static size_t ssl_transform_get_explicit_iv_len(
                        mbedtls_ssl_transform const *transform )
{
    if( transform->minor_ver < MBEDTLS_SSL_MINOR_VERSION_2 )
        return( 0 );

    return( transform->ivlen - transform->fixed_ivlen );
}