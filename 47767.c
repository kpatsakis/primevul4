int ssl_cipher_list_to_bytes(SSL *s, STACK_OF(SSL_CIPHER) *sk,
                             unsigned char *p,
                             int (*put_cb) (const SSL_CIPHER *,
                                            unsigned char *))
{
    int i, j = 0;
    SSL_CIPHER *c;
    unsigned char *q;
    int empty_reneg_info_scsv = !s->renegotiate;
    /* Set disabled masks for this session */
    ssl_set_client_disabled(s);

    if (sk == NULL)
        return (0);
    q = p;
    if (put_cb == NULL)
        put_cb = s->method->put_cipher_by_char;

    for (i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
        c = sk_SSL_CIPHER_value(sk, i);
        /* Skip disabled ciphers */
        if (ssl_cipher_disabled(s, c, SSL_SECOP_CIPHER_SUPPORTED))
            continue;
#ifdef OPENSSL_SSL_DEBUG_BROKEN_PROTOCOL
        if (c->id == SSL3_CK_SCSV) {
            if (!empty_reneg_info_scsv)
                continue;
            else
                empty_reneg_info_scsv = 0;
        }
#endif
        j = put_cb(c, p);
        p += j;
    }
    /*
     * If p == q, no ciphers; caller indicates an error. Otherwise, add
     * applicable SCSVs.
     */
    if (p != q) {
        if (empty_reneg_info_scsv) {
            static SSL_CIPHER scsv = {
                0, NULL, SSL3_CK_SCSV, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            j = put_cb(&scsv, p);
            p += j;
#ifdef OPENSSL_RI_DEBUG
            fprintf(stderr,
                    "TLS_EMPTY_RENEGOTIATION_INFO_SCSV sent by client\n");
#endif
        }
        if (s->mode & SSL_MODE_SEND_FALLBACK_SCSV) {
            static SSL_CIPHER scsv = {
                0, NULL, SSL3_CK_FALLBACK_SCSV, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            j = put_cb(&scsv, p);
            p += j;
        }
    }

    return (p - q);
}
