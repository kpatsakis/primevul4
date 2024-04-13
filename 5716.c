static int process_signals(void)
{
	if (sig_exit_handler_called) {
		log_info("caught signal %d", sig_exit_handler_sig);
		return 1;
	}
	if (sig_usr1_handler_called) {
		sig_usr1_handler_called = 0;
		tickets_log_info();
	}
	if (sig_chld_handler_called) {
		sig_chld_handler_called = 0;
		wait_child(SIGCHLD);
	}

	return 0;
}