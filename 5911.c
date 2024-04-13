int mbedtls_ssl_check_timer( mbedtls_ssl_context *ssl )
{
    if( ssl->f_get_timer == NULL )
        return( 0 );

    if( ssl->f_get_timer( ssl->p_timer ) == 2 )
    {
        MBEDTLS_SSL_DEBUG_MSG( 3, ( "timer expired" ) );
        return( -1 );
    }

    return( 0 );
}