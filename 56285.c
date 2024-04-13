ether_if_print(netdissect_options *ndo, const struct pcap_pkthdr *h,
               const u_char *p)
{
	return (ether_print(ndo, p, h->len, h->caplen, NULL, NULL));
}
