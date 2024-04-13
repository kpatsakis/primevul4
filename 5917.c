static void ssl_build_record_nonce( unsigned char *dst_iv,
                                    size_t dst_iv_len,
                                    unsigned char const *fixed_iv,
                                    size_t fixed_iv_len,
                                    unsigned char const *dynamic_iv,
                                    size_t dynamic_iv_len )
{
    size_t i;

    /* Start with Fixed IV || 0 */
    memset( dst_iv, 0, dst_iv_len );
    memcpy( dst_iv, fixed_iv, fixed_iv_len );

    dst_iv += dst_iv_len - dynamic_iv_len;
    for( i = 0; i < dynamic_iv_len; i++ )
        dst_iv[i] ^= dynamic_iv[i];
}