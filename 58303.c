void enl_ipc_send(char *str)
{

	static char *last_msg = NULL;
	char buff[21];
	register unsigned short i;
	register unsigned char j;
	unsigned short len;
	XEvent ev;

	if (str == NULL) {
		if (last_msg == NULL)
			eprintf("eeek");
		str = last_msg;
		D(("Resending last message \"%s\" to Enlightenment.\n", str));
	} else {
		if (last_msg != NULL) {
			free(last_msg);
		}
		last_msg = estrdup(str);
		D(("Sending \"%s\" to Enlightenment.\n", str));
	}
	if (ipc_win == None) {
		if ((ipc_win = enl_ipc_get_win()) == None) {
			D(("Hrm. Enlightenment doesn't seem to be running. No IPC window, no IPC.\n"));
			return;
		}
	}
	len = strlen(str);
	ipc_atom = XInternAtom(disp, "ENL_MSG", False);
	if (ipc_atom == None) {
		D(("IPC error:  Unable to find/create ENL_MSG atom.\n"));
		return;
	}
	for (; XCheckTypedWindowEvent(disp, my_ipc_win, ClientMessage, &ev););	/* Discard any out-of-sync messages */
	ev.xclient.type = ClientMessage;
	ev.xclient.serial = 0;
	ev.xclient.send_event = True;
	ev.xclient.window = ipc_win;
	ev.xclient.message_type = ipc_atom;
	ev.xclient.format = 8;

	for (i = 0; i < len + 1; i += 12) {
		sprintf(buff, "%8x", (int) my_ipc_win);
		for (j = 0; j < 12; j++) {
			buff[8 + j] = str[i + j];
			if (!str[i + j]) {
				break;
			}
		}
		buff[20] = 0;
		for (j = 0; j < 20; j++) {
			ev.xclient.data.b[j] = buff[j];
		}
		XSendEvent(disp, ipc_win, False, 0, (XEvent *) & ev);
	}
	return;
}
