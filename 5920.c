int mbedtls_ssl_parse_change_cipher_spec( mbedtls_ssl_context *ssl )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "=> parse change cipher spec" ) );

    if( ( ret = mbedtls_ssl_read_record( ssl, 1 ) ) != 0 )
    {
        MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_read_record", ret );
        return( ret );
    }

    if( ssl->in_msgtype != MBEDTLS_SSL_MSG_CHANGE_CIPHER_SPEC )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "bad change cipher spec message" ) );
        mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                        MBEDTLS_SSL_ALERT_MSG_UNEXPECTED_MESSAGE );
        return( MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE );
    }

    /* CCS records are only accepted if they have length 1 and content '1',
     * so we don't need to check this here. */

    /*
     * Switch to our negotiated transform and session parameters for inbound
     * data.
     */
    MBEDTLS_SSL_DEBUG_MSG( 3, ( "switching to new transform spec for inbound data" ) );
    ssl->transform_in = ssl->transform_negotiate;
    ssl->session_in = ssl->session_negotiate;

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM )
    {
#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
        mbedtls_ssl_dtls_replay_reset( ssl );
#endif

        /* Increment epoch */
        if( ++ssl->in_epoch == 0 )
        {
            MBEDTLS_SSL_DEBUG_MSG( 1, ( "DTLS epoch would wrap" ) );
            /* This is highly unlikely to happen for legitimate reasons, so
               treat it as an attack and don't send an alert. */
            return( MBEDTLS_ERR_SSL_COUNTER_WRAPPING );
        }
    }
    else
#endif /* MBEDTLS_SSL_PROTO_DTLS */
    memset( ssl->in_ctr, 0, 8 );

    mbedtls_ssl_update_in_pointers( ssl );

#if defined(MBEDTLS_SSL_HW_RECORD_ACCEL)
    if( mbedtls_ssl_hw_record_activate != NULL )
    {
        if( ( ret = mbedtls_ssl_hw_record_activate( ssl, MBEDTLS_SSL_CHANNEL_INBOUND ) ) != 0 )
        {
            MBEDTLS_SSL_DEBUG_RET( 1, "mbedtls_ssl_hw_record_activate", ret );
            mbedtls_ssl_send_alert_message( ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL,
                                            MBEDTLS_SSL_ALERT_MSG_INTERNAL_ERROR );
            return( MBEDTLS_ERR_SSL_HW_ACCEL_FAILED );
        }
    }
#endif

    ssl->state++;

    MBEDTLS_SSL_DEBUG_MSG( 2, ( "<= parse change cipher spec" ) );

    return( 0 );
}