static void sig_exit_handler(int sig)
{
	sig_exit_handler_sig = sig;
	sig_exit_handler_called = 1;
}