static size_t ssl_compute_padding_length( size_t len,
                                          size_t granularity )
{
    return( ( granularity - ( len + 1 ) % granularity ) % granularity );
}