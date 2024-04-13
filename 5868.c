static int ssl_parse_inner_plaintext( unsigned char const *content,
                                          size_t *content_size,
                                          uint8_t *rec_type )
{
    size_t remaining = *content_size;

    /* Determine length of padding by skipping zeroes from the back. */
    do
    {
        if( remaining == 0 )
            return( -1 );
        remaining--;
    } while( content[ remaining ] == 0 );

    *content_size = remaining;
    *rec_type = content[ remaining ];

    return( 0 );
}