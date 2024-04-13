static int __init hidp_init(void)
{
	BT_INFO("HIDP (Human Interface Emulation) ver %s", VERSION);

	return hidp_init_sockets();
}
