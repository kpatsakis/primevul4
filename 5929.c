static int ssl_build_inner_plaintext( unsigned char *content,
                                      size_t *content_size,
                                      size_t remaining,
                                      uint8_t rec_type,
                                      size_t pad )
{
    size_t len = *content_size;

    /* Write real content type */
    if( remaining == 0 )
        return( -1 );
    content[ len ] = rec_type;
    len++;
    remaining--;

    if( remaining < pad )
        return( -1 );
    memset( content + len, 0, pad );
    len += pad;
    remaining -= pad;

    *content_size = len;
    return( 0 );
}