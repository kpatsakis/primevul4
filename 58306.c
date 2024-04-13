char *enl_wait_for_reply(void)
{

	XEvent ev;
	static char msg_buffer[20];
	register unsigned char i;

	alarm(2);
	for (; !XCheckTypedWindowEvent(disp, my_ipc_win, ClientMessage, &ev)
	     && !timeout;);
	alarm(0);
	if (ev.xany.type != ClientMessage) {
		return(IPC_TIMEOUT);
	}
	for (i = 0; i < 20; i++) {
		msg_buffer[i] = ev.xclient.data.b[i];
	}
	return(msg_buffer + 8);
}
