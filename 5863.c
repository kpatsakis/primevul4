int mbedtls_ssl_send_fatal_handshake_failure( mbedtls_ssl_context *ssl )
{
    return( mbedtls_ssl_send_alert_message( ssl,
                  MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                  MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE ) );
}