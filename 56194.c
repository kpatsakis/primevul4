ieee802_11_if_print(netdissect_options *ndo,
                    const struct pcap_pkthdr *h, const u_char *p)
{
	return ieee802_11_print(ndo, p, h->len, h->caplen, 0, 0);
}
