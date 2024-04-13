int ssl3_get_server_certificate(SSL *s)
{
    int al, i, ok, ret = -1, exp_idx;
    unsigned long n, nc, llen, l;
    X509 *x = NULL;
    const unsigned char *q, *p;
    unsigned char *d;
    STACK_OF(X509) *sk = NULL;
    SESS_CERT *sc;
    EVP_PKEY *pkey = NULL;

    n = s->method->ssl_get_message(s,
                                   SSL3_ST_CR_CERT_A,
                                   SSL3_ST_CR_CERT_B,
                                   -1, s->max_cert_list, &ok);

    if (!ok)
        return ((int)n);

    if (s->s3->tmp.message_type == SSL3_MT_SERVER_KEY_EXCHANGE) {
        s->s3->tmp.reuse_message = 1;
        return (1);
    }

    if (s->s3->tmp.message_type != SSL3_MT_CERTIFICATE) {
        al = SSL_AD_UNEXPECTED_MESSAGE;
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE, SSL_R_BAD_MESSAGE_TYPE);
        goto f_err;
    }
    p = d = (unsigned char *)s->init_msg;

    if ((sk = sk_X509_new_null()) == NULL) {
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    n2l3(p, llen);
    if (llen + 3 != n) {
        al = SSL_AD_DECODE_ERROR;
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE, SSL_R_LENGTH_MISMATCH);
        goto f_err;
    }
    for (nc = 0; nc < llen;) {
        n2l3(p, l);
        if ((l + nc + 3) > llen) {
            al = SSL_AD_DECODE_ERROR;
            SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE,
                   SSL_R_CERT_LENGTH_MISMATCH);
            goto f_err;
        }

        q = p;
        x = d2i_X509(NULL, &q, l);
        if (x == NULL) {
            al = SSL_AD_BAD_CERTIFICATE;
            SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE, ERR_R_ASN1_LIB);
            goto f_err;
        }
        if (q != (p + l)) {
            al = SSL_AD_DECODE_ERROR;
            SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE,
                   SSL_R_CERT_LENGTH_MISMATCH);
            goto f_err;
        }
        if (!sk_X509_push(sk, x)) {
            SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE, ERR_R_MALLOC_FAILURE);
            goto err;
        }
        x = NULL;
        nc += l + 3;
        p = q;
    }

    i = ssl_verify_cert_chain(s, sk);
    if (s->verify_mode != SSL_VERIFY_NONE && i <= 0) {
        al = ssl_verify_alarm_type(s->verify_result);
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE,
               SSL_R_CERTIFICATE_VERIFY_FAILED);
        goto f_err;
    }
    ERR_clear_error();          /* but we keep s->verify_result */
    if (i > 1) {
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE, i);
        al = SSL_AD_HANDSHAKE_FAILURE;
        goto f_err;
    }

    sc = ssl_sess_cert_new();
    if (sc == NULL)
        goto err;

    ssl_sess_cert_free(s->session->sess_cert);
    s->session->sess_cert = sc;

    sc->cert_chain = sk;
    /*
     * Inconsistency alert: cert_chain does include the peer's certificate,
     * which we don't include in s3_srvr.c
     */
    x = sk_X509_value(sk, 0);
    sk = NULL;
    /*
     * VRS 19990621: possible memory leak; sk=null ==> !sk_pop_free() @end
     */

    pkey = X509_get_pubkey(x);

    if (pkey == NULL || EVP_PKEY_missing_parameters(pkey)) {
        x = NULL;
        al = SSL3_AL_FATAL;
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE,
               SSL_R_UNABLE_TO_FIND_PUBLIC_KEY_PARAMETERS);
        goto f_err;
    }

    i = ssl_cert_type(x, pkey);
    if (i < 0) {
        x = NULL;
        al = SSL3_AL_FATAL;
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE,
               SSL_R_UNKNOWN_CERTIFICATE_TYPE);
        goto f_err;
    }

    exp_idx = ssl_cipher_get_cert_index(s->s3->tmp.new_cipher);
    if (exp_idx >= 0 && i != exp_idx) {
        x = NULL;
        al = SSL_AD_ILLEGAL_PARAMETER;
        SSLerr(SSL_F_SSL3_GET_SERVER_CERTIFICATE,
               SSL_R_WRONG_CERTIFICATE_TYPE);
        goto f_err;
    }
    sc->peer_cert_type = i;
    CRYPTO_add(&x->references, 1, CRYPTO_LOCK_X509);
    /*
     * Why would the following ever happen? We just created sc a couple
     * of lines ago.
     */
    X509_free(sc->peer_pkeys[i].x509);
    sc->peer_pkeys[i].x509 = x;
    sc->peer_key = &(sc->peer_pkeys[i]);

    X509_free(s->session->peer);
    CRYPTO_add(&x->references, 1, CRYPTO_LOCK_X509);
    s->session->peer = x;
    s->session->verify_result = s->verify_result;

    x = NULL;
    ret = 1;
    goto done;

 f_err:
    ssl3_send_alert(s, SSL3_AL_FATAL, al);
 err:
    s->state = SSL_ST_ERR;
 done:
    EVP_PKEY_free(pkey);
    X509_free(x);
    sk_X509_pop_free(sk, X509_free);
    return (ret);
}
