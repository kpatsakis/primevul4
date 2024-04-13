static void __exit hidp_exit(void)
{
	hidp_cleanup_sockets();
}
