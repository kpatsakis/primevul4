static int ssl_check_record_type( uint8_t record_type )
{
    if( record_type != MBEDTLS_SSL_MSG_HANDSHAKE &&
        record_type != MBEDTLS_SSL_MSG_ALERT &&
        record_type != MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC &&
        record_type != MBEDTLS_SSL_MSG_APPLICATION_DATA )
    {
        return( MBEDTLS_ERR_SSL_INVALID_RECORD );
    }

    return( 0 );
}