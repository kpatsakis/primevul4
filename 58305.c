char *enl_send_and_wait(char *msg)
{
	char *reply = IPC_TIMEOUT;
	sighandler_t old_alrm;

	/*
	 * Shortcut this func and return IPC_FAKE
	 * If the IPC Window is the E17 fake
	 */
	if (e17_fake_ipc)
		return IPC_FAKE;

	if (ipc_win == None) {
		/* The IPC window is missing.  Wait for it to return or feh to be killed. */
		/* Only called once in the E17 case */
		for (; enl_ipc_get_win() == None;) {
			if (e17_fake_ipc)
				return IPC_FAKE;
			else
				sleep(1);
		}
	}
	old_alrm = (sighandler_t) signal(SIGALRM, (sighandler_t) enl_ipc_timeout);
	for (; reply == IPC_TIMEOUT;) {
		timeout = 0;
		enl_ipc_send(msg);
		for (; !(reply = enl_ipc_get(enl_wait_for_reply())););
		if (reply == IPC_TIMEOUT) {
			/* We timed out.  The IPC window must be AWOL.  Reset and resend message. */
			D(("IPC timed out.  IPC window has gone. Clearing ipc_win.\n"));
			XSelectInput(disp, ipc_win, None);
			ipc_win = None;
		}
	}
	signal(SIGALRM, old_alrm);
	return(reply);
}
