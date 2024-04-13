setpassfilter(argv)
    char **argv;
{
    pcap_t *pc;
    int ret = 1;

    pc = pcap_open_dead(DLT_PPP_PPPD, 65535);
    if (pcap_compile(pc, &pass_filter, *argv, 1, netmask) == -1) {
	option_error("error in pass-filter expression: %s\n",
		     pcap_geterr(pc));
	ret = 0;
    }
    pcap_close(pc);

    return ret;
}
