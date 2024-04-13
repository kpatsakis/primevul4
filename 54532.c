_kdc_is_anon_request(const KDC_REQ_BODY *b)
{
	/* some versions of heimdal use bit 14 instead of 16 for
	   request_anonymous, as indicated in the anonymous draft prior to
	   version 11. Bit 14 is assigned to S4U2Proxy, but all S4U2Proxy
	   requests will have a second ticket; don't consider those anonymous */
	return (b->kdc_options.request_anonymous ||
		(b->kdc_options.constrained_delegation && !b->additional_tickets));
}
