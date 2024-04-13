static int __init bnep_init(void)
{
	char flt[50] = "";

#ifdef CONFIG_BT_BNEP_PROTO_FILTER
	strcat(flt, "protocol ");
#endif

#ifdef CONFIG_BT_BNEP_MC_FILTER
	strcat(flt, "multicast");
#endif

	BT_INFO("BNEP (Ethernet Emulation) ver %s", VERSION);
	if (flt[0])
		BT_INFO("BNEP filters: %s", flt);

	bnep_sock_init();
	return 0;
}
