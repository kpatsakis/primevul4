int SSL_set_session_ticket_ext(SSL *s, void *ext_data, int ext_len)
{
    if (s->version >= TLS1_VERSION) {
        OPENSSL_free(s->tlsext_session_ticket);
        s->tlsext_session_ticket = NULL;
        s->tlsext_session_ticket =
            OPENSSL_malloc(sizeof(TLS_SESSION_TICKET_EXT) + ext_len);
        if (!s->tlsext_session_ticket) {
            SSLerr(SSL_F_SSL_SET_SESSION_TICKET_EXT, ERR_R_MALLOC_FAILURE);
            return 0;
        }

        if (ext_data) {
            s->tlsext_session_ticket->length = ext_len;
            s->tlsext_session_ticket->data = s->tlsext_session_ticket + 1;
            memcpy(s->tlsext_session_ticket->data, ext_data, ext_len);
        } else {
            s->tlsext_session_ticket->length = 0;
            s->tlsext_session_ticket->data = NULL;
        }

        return 1;
    }

    return 0;
}
