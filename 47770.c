unsigned long SSL_SESSION_get_ticket_lifetime_hint(const SSL_SESSION *s)
{
    return s->tlsext_tick_lifetime_hint;
}
