static int ssl_mac( mbedtls_md_context_t *md_ctx,
                    const unsigned char *secret,
                    const unsigned char *buf, size_t len,
                    const unsigned char *ctr, int type,
                    unsigned char out[SSL3_MAC_MAX_BYTES] )
{
    unsigned char header[11];
    unsigned char padding[48];
    int padlen;
    int md_size = mbedtls_md_get_size( md_ctx->md_info );
    int md_type = mbedtls_md_get_type( md_ctx->md_info );
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    /* Only MD5 and SHA-1 supported */
    if( md_type == MBEDTLS_MD_MD5 )
        padlen = 48;
    else
        padlen = 40;

    memcpy( header, ctr, 8 );
    header[8] = (unsigned char)  type;
    MBEDTLS_PUT_UINT16_BE( len, header, 9);

    memset( padding, 0x36, padlen );
    ret = mbedtls_md_starts( md_ctx );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, secret,  md_size );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, padding, padlen  );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, header,  11      );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, buf,     len     );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_finish( md_ctx, out              );
    if( ret != 0 )
        return( ret );

    memset( padding, 0x5C, padlen );
    ret = mbedtls_md_starts( md_ctx );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, secret,    md_size );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, padding,   padlen  );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_update( md_ctx, out,       md_size );
    if( ret != 0 )
        return( ret );
    ret = mbedtls_md_finish( md_ctx, out                );
    if( ret != 0 )
        return( ret );

    return( 0 );
}