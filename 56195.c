ieee802_11_radio_if_print(netdissect_options *ndo,
                          const struct pcap_pkthdr *h, const u_char *p)
{
	return ieee802_11_radio_print(ndo, p, h->len, h->caplen);
}
