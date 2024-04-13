void SSL_SESSION_get0_ticket(const SSL_SESSION *s, unsigned char **tick,
                            size_t *len)
{
    *len = s->tlsext_ticklen;
    if (tick != NULL)
        *tick = s->tlsext_tick;
}
