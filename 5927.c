static int ssl_transform_aead_dynamic_iv_is_explicit(
                                mbedtls_ssl_transform const *transform )
{
    return( transform->ivlen != transform->fixed_ivlen );
}