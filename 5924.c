static int mbedtls_ssl_dtls_record_replay_check( mbedtls_ssl_context *ssl, uint8_t *record_in_ctr )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char *original_in_ctr;

    // save original in_ctr
    original_in_ctr = ssl->in_ctr;

    // use counter from record
    ssl->in_ctr = record_in_ctr;

    ret = mbedtls_ssl_dtls_replay_check( (mbedtls_ssl_context const *) ssl );

    // restore the counter
    ssl->in_ctr = original_in_ctr;

    return ret;
}