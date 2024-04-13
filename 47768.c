static IMPLEMENT_LHASH_DOALL_ARG_FN(timeout, SSL_SESSION, TIMEOUT_PARAM)

void SSL_CTX_flush_sessions(SSL_CTX *s, long t)
{
    unsigned long i;
    TIMEOUT_PARAM tp;

    tp.ctx = s;
    tp.cache = s->sessions;
    if (tp.cache == NULL)
        return;
    tp.time = t;
    CRYPTO_w_lock(CRYPTO_LOCK_SSL_CTX);
    i = CHECKED_LHASH_OF(SSL_SESSION, tp.cache)->down_load;
    CHECKED_LHASH_OF(SSL_SESSION, tp.cache)->down_load = 0;
    lh_SSL_SESSION_doall_arg(tp.cache, LHASH_DOALL_ARG_FN(timeout),
                             TIMEOUT_PARAM, &tp);
    CHECKED_LHASH_OF(SSL_SESSION, tp.cache)->down_load = i;
    CRYPTO_w_unlock(CRYPTO_LOCK_SSL_CTX);
}
