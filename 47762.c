int ssl3_get_server_done(SSL *s)
{
    int ok, ret = 0;
    long n;

    /* Second to last param should be very small, like 0 :-) */
    n = s->method->ssl_get_message(s,
                                   SSL3_ST_CR_SRVR_DONE_A,
                                   SSL3_ST_CR_SRVR_DONE_B,
                                   SSL3_MT_SERVER_DONE, 30, &ok);

    if (!ok)
        return ((int)n);
    if (n > 0) {
        /* should contain no data */
        ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
        SSLerr(SSL_F_SSL3_GET_SERVER_DONE, SSL_R_LENGTH_MISMATCH);
        s->state = SSL_ST_ERR;
        return -1;
    }
    ret = 1;
    return (ret);
}
