int ssl_get_prev_session(SSL *s, unsigned char *session_id, int len,
                         const unsigned char *limit)
{
    /* This is used only by servers. */

    SSL_SESSION *ret = NULL;
    int fatal = 0;
    int try_session_cache = 1;
    int r;

    if (len < 0 || len > SSL_MAX_SSL_SESSION_ID_LENGTH)
        goto err;

    if (session_id + len > limit) {
        fatal = 1;
        goto err;
    }

    if (len == 0)
        try_session_cache = 0;

    /* sets s->tlsext_ticket_expected */
    r = tls1_process_ticket(s, session_id, len, limit, &ret);
    switch (r) {
    case -1:                   /* Error during processing */
        fatal = 1;
        goto err;
    case 0:                    /* No ticket found */
    case 1:                    /* Zero length ticket found */
        break;                  /* Ok to carry on processing session id. */
    case 2:                    /* Ticket found but not decrypted. */
    case 3:                    /* Ticket decrypted, *ret has been set. */
        try_session_cache = 0;
        break;
    default:
        abort();
    }

    if (try_session_cache &&
        ret == NULL &&
        !(s->session_ctx->session_cache_mode &
          SSL_SESS_CACHE_NO_INTERNAL_LOOKUP)) {
        SSL_SESSION data;
        data.ssl_version = s->version;
        data.session_id_length = len;
        if (len == 0)
            return 0;
        memcpy(data.session_id, session_id, len);
        CRYPTO_r_lock(CRYPTO_LOCK_SSL_CTX);
        ret = lh_SSL_SESSION_retrieve(s->session_ctx->sessions, &data);
        if (ret != NULL) {
            /* don't allow other threads to steal it: */
            CRYPTO_add(&ret->references, 1, CRYPTO_LOCK_SSL_SESSION);
        }
        CRYPTO_r_unlock(CRYPTO_LOCK_SSL_CTX);
        if (ret == NULL)
            s->session_ctx->stats.sess_miss++;
    }

    if (try_session_cache &&
        ret == NULL && s->session_ctx->get_session_cb != NULL) {
        int copy = 1;

        if ((ret = s->session_ctx->get_session_cb(s, session_id, len, &copy))) {
            s->session_ctx->stats.sess_cb_hit++;

            /*
             * Increment reference count now if the session callback asks us
             * to do so (note that if the session structures returned by the
             * callback are shared between threads, it must handle the
             * reference count itself [i.e. copy == 0], or things won't be
             * thread-safe).
             */
            if (copy)
                CRYPTO_add(&ret->references, 1, CRYPTO_LOCK_SSL_SESSION);

            /*
             * Add the externally cached session to the internal cache as
             * well if and only if we are supposed to.
             */
            if (!
                (s->session_ctx->session_cache_mode &
                 SSL_SESS_CACHE_NO_INTERNAL_STORE)) {
                /*
                 * The following should not return 1, otherwise, things are
                 * very strange
                 */
                if (SSL_CTX_add_session(s->session_ctx, ret))
                    goto err;
            }
        }
    }

    if (ret == NULL)
        goto err;

    /* Now ret is non-NULL and we own one of its reference counts. */

    if (ret->sid_ctx_length != s->sid_ctx_length
        || memcmp(ret->sid_ctx, s->sid_ctx, ret->sid_ctx_length)) {
        /*
         * We have the session requested by the client, but we don't want to
         * use it in this context.
         */
        goto err;               /* treat like cache miss */
    }

    if ((s->verify_mode & SSL_VERIFY_PEER) && s->sid_ctx_length == 0) {
        /*
         * We can't be sure if this session is being used out of context,
         * which is especially important for SSL_VERIFY_PEER. The application
         * should have used SSL[_CTX]_set_session_id_context. For this error
         * case, we generate an error instead of treating the event like a
         * cache miss (otherwise it would be easy for applications to
         * effectively disable the session cache by accident without anyone
         * noticing).
         */

        SSLerr(SSL_F_SSL_GET_PREV_SESSION,
               SSL_R_SESSION_ID_CONTEXT_UNINITIALIZED);
        fatal = 1;
        goto err;
    }

    if (ret->cipher == NULL) {
        unsigned char buf[5], *p;
        unsigned long l;

        p = buf;
        l = ret->cipher_id;
        l2n(l, p);
        if ((ret->ssl_version >> 8) >= SSL3_VERSION_MAJOR)
            ret->cipher = ssl_get_cipher_by_char(s, &(buf[2]));
        else
            ret->cipher = ssl_get_cipher_by_char(s, &(buf[1]));
        if (ret->cipher == NULL)
            goto err;
    }

    if (ret->timeout < (long)(time(NULL) - ret->time)) { /* timeout */
        s->session_ctx->stats.sess_timeout++;
        if (try_session_cache) {
            /* session was from the cache, so remove it */
            SSL_CTX_remove_session(s->session_ctx, ret);
        }
        goto err;
    }

    s->session_ctx->stats.sess_hit++;

    SSL_SESSION_free(s->session);
    s->session = ret;
    s->verify_result = s->session->verify_result;
    return 1;

 err:
    if (ret != NULL) {
        SSL_SESSION_free(ret);

        if (!try_session_cache) {
            /*
             * The session was from a ticket, so we should issue a ticket for
             * the new session
             */
            s->tlsext_ticket_expected = 1;
        }
    }
    if (fatal)
        return -1;
    else
        return 0;
}
