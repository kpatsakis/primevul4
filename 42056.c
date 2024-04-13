static bool try_mac(const node_t *n, const vpn_packet_t *inpkt) {
	unsigned char hmac[EVP_MAX_MD_SIZE];

	if(!n->indigest || !n->inmaclength || !n->inkey || inpkt->len < sizeof inpkt->seqno + n->inmaclength)
		return false;

	HMAC(n->indigest, n->inkey, n->inkeylength, (unsigned char *) &inpkt->seqno, inpkt->len - n->inmaclength, (unsigned char *)hmac, NULL);

	return !memcmp(hmac, (char *) &inpkt->seqno + inpkt->len - n->inmaclength, n->inmaclength);
}
