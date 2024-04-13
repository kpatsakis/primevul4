int ssl3_get_cert_status(SSL *s)
{
    int ok, al;
    unsigned long resplen, n;
    const unsigned char *p;

    n = s->method->ssl_get_message(s,
                                   SSL3_ST_CR_CERT_STATUS_A,
                                   SSL3_ST_CR_CERT_STATUS_B,
                                   SSL3_MT_CERTIFICATE_STATUS, 16384, &ok);

    if (!ok)
        return ((int)n);
    if (n < 4) {
        /* need at least status type + length */
        al = SSL_AD_DECODE_ERROR;
        SSLerr(SSL_F_SSL3_GET_CERT_STATUS, SSL_R_LENGTH_MISMATCH);
        goto f_err;
    }
    p = (unsigned char *)s->init_msg;
    if (*p++ != TLSEXT_STATUSTYPE_ocsp) {
        al = SSL_AD_DECODE_ERROR;
        SSLerr(SSL_F_SSL3_GET_CERT_STATUS, SSL_R_UNSUPPORTED_STATUS_TYPE);
        goto f_err;
    }
    n2l3(p, resplen);
    if (resplen + 4 != n) {
        al = SSL_AD_DECODE_ERROR;
        SSLerr(SSL_F_SSL3_GET_CERT_STATUS, SSL_R_LENGTH_MISMATCH);
        goto f_err;
    }
    OPENSSL_free(s->tlsext_ocsp_resp);
    s->tlsext_ocsp_resp = BUF_memdup(p, resplen);
    if (!s->tlsext_ocsp_resp) {
        al = SSL_AD_INTERNAL_ERROR;
        SSLerr(SSL_F_SSL3_GET_CERT_STATUS, ERR_R_MALLOC_FAILURE);
        goto f_err;
    }
    s->tlsext_ocsp_resplen = resplen;
    if (s->ctx->tlsext_status_cb) {
        int ret;
        ret = s->ctx->tlsext_status_cb(s, s->ctx->tlsext_status_arg);
        if (ret == 0) {
            al = SSL_AD_BAD_CERTIFICATE_STATUS_RESPONSE;
            SSLerr(SSL_F_SSL3_GET_CERT_STATUS, SSL_R_INVALID_STATUS_RESPONSE);
            goto f_err;
        }
        if (ret < 0) {
            al = SSL_AD_INTERNAL_ERROR;
            SSLerr(SSL_F_SSL3_GET_CERT_STATUS, ERR_R_MALLOC_FAILURE);
            goto f_err;
        }
    }
    return 1;
 f_err:
    ssl3_send_alert(s, SSL3_AL_FATAL, al);
    s->state = SSL_ST_ERR;
    return (-1);
}
