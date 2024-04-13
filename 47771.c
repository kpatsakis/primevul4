int SSL_SESSION_has_ticket(const SSL_SESSION *s)
{
    return (s->tlsext_ticklen > 0) ? 1 : 0;
}
