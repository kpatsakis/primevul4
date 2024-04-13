require_preauth_p(kdc_request_t r)
{
    return r->config->require_preauth
	|| r->client->entry.flags.require_preauth
	|| r->server->entry.flags.require_preauth;
}
