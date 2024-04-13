static int __init nf_nat_irc_init(void)
{
	BUG_ON(nf_nat_irc_hook != NULL);
	RCU_INIT_POINTER(nf_nat_irc_hook, help);
	return 0;
}
