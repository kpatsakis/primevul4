int ssl_get_new_session(SSL *s, int session)
{
    /* This gets used by clients and servers. */

    unsigned int tmp;
    SSL_SESSION *ss = NULL;
    GEN_SESSION_CB cb = def_generate_session_id;

    if ((ss = SSL_SESSION_new()) == NULL)
        return (0);

    /* If the context has a default timeout, use it */
    if (s->session_ctx->session_timeout == 0)
        ss->timeout = SSL_get_default_timeout(s);
    else
        ss->timeout = s->session_ctx->session_timeout;

    SSL_SESSION_free(s->session);
    s->session = NULL;

    if (session) {
        if (s->version == SSL3_VERSION) {
            ss->ssl_version = SSL3_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == TLS1_VERSION) {
            ss->ssl_version = TLS1_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == TLS1_1_VERSION) {
            ss->ssl_version = TLS1_1_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == TLS1_2_VERSION) {
            ss->ssl_version = TLS1_2_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == DTLS1_BAD_VER) {
            ss->ssl_version = DTLS1_BAD_VER;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == DTLS1_VERSION) {
            ss->ssl_version = DTLS1_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else if (s->version == DTLS1_2_VERSION) {
            ss->ssl_version = DTLS1_2_VERSION;
            ss->session_id_length = SSL3_SSL_SESSION_ID_LENGTH;
        } else {
            SSLerr(SSL_F_SSL_GET_NEW_SESSION, SSL_R_UNSUPPORTED_SSL_VERSION);
            SSL_SESSION_free(ss);
            return (0);
        }

        /*-
         * If RFC5077 ticket, use empty session ID (as server).
         * Note that:
         * (a) ssl_get_prev_session() does lookahead into the
         *     ClientHello extensions to find the session ticket.
         *     When ssl_get_prev_session() fails, s3_srvr.c calls
         *     ssl_get_new_session() in ssl3_get_client_hello().
         *     At that point, it has not yet parsed the extensions,
         *     however, because of the lookahead, it already knows
         *     whether a ticket is expected or not.
         *
         * (b) s3_clnt.c calls ssl_get_new_session() before parsing
         *     ServerHello extensions, and before recording the session
         *     ID received from the server, so this block is a noop.
         */
        if (s->tlsext_ticket_expected) {
            ss->session_id_length = 0;
            goto sess_id_done;
        }

        /* Choose which callback will set the session ID */
        CRYPTO_r_lock(CRYPTO_LOCK_SSL_CTX);
        if (s->generate_session_id)
            cb = s->generate_session_id;
        else if (s->session_ctx->generate_session_id)
            cb = s->session_ctx->generate_session_id;
        CRYPTO_r_unlock(CRYPTO_LOCK_SSL_CTX);
        /* Choose a session ID */
        tmp = ss->session_id_length;
        if (!cb(s, ss->session_id, &tmp)) {
            /* The callback failed */
            SSLerr(SSL_F_SSL_GET_NEW_SESSION,
                   SSL_R_SSL_SESSION_ID_CALLBACK_FAILED);
            SSL_SESSION_free(ss);
            return (0);
        }
        /*
         * Don't allow the callback to set the session length to zero. nor
         * set it higher than it was.
         */
        if (!tmp || (tmp > ss->session_id_length)) {
            /* The callback set an illegal length */
            SSLerr(SSL_F_SSL_GET_NEW_SESSION,
                   SSL_R_SSL_SESSION_ID_HAS_BAD_LENGTH);
            SSL_SESSION_free(ss);
            return (0);
        }
        ss->session_id_length = tmp;
        /* Finally, check for a conflict */
        if (SSL_has_matching_session_id(s, ss->session_id,
                                        ss->session_id_length)) {
            SSLerr(SSL_F_SSL_GET_NEW_SESSION, SSL_R_SSL_SESSION_ID_CONFLICT);
            SSL_SESSION_free(ss);
            return (0);
        }

 sess_id_done:
        if (s->tlsext_hostname) {
            ss->tlsext_hostname = BUF_strdup(s->tlsext_hostname);
            if (ss->tlsext_hostname == NULL) {
                SSLerr(SSL_F_SSL_GET_NEW_SESSION, ERR_R_INTERNAL_ERROR);
                SSL_SESSION_free(ss);
                return 0;
            }
        }
    } else {
        ss->session_id_length = 0;
    }

    if (s->sid_ctx_length > sizeof ss->sid_ctx) {
        SSLerr(SSL_F_SSL_GET_NEW_SESSION, ERR_R_INTERNAL_ERROR);
        SSL_SESSION_free(ss);
        return 0;
    }
    memcpy(ss->sid_ctx, s->sid_ctx, s->sid_ctx_length);
    ss->sid_ctx_length = s->sid_ctx_length;
    s->session = ss;
    ss->ssl_version = s->version;
    ss->verify_result = X509_V_OK;

    return (1);
}
