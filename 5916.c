static size_t ssl_get_reassembly_buffer_size( size_t msg_len,
                                              unsigned add_bitmap )
{
    size_t alloc_len;

    alloc_len  = 12;                                 /* Handshake header */
    alloc_len += msg_len;                            /* Content buffer   */

    if( add_bitmap )
        alloc_len += msg_len / 8 + ( msg_len % 8 != 0 ); /* Bitmap       */

    return( alloc_len );
}