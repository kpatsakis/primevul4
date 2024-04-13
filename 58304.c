static sighandler_t *enl_ipc_timeout(int sig)
{
	timeout = 1;
	return((sighandler_t *) sig);
}
