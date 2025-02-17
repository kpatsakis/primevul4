Window enl_ipc_get_win(void)
{

	unsigned char *str = NULL;
	Atom prop, prop2, ever;
	unsigned long num, after;
	int format;
	Window dummy_win;
	int dummy_int;
	unsigned int dummy_uint;

	D(("Searching for IPC window.\n"));

	/*
	 * Shortcircuit this entire func
	 * if we already know it's an e17 fake
	 */
	if (e17_fake_ipc)
		return(ipc_win);

		    prop = XInternAtom(disp, "ENLIGHTENMENT_COMMS", True);
	if (prop == None) {
		D(("Enlightenment is not running.\n"));
		return(None);
	} else {
		/* XXX: This will only work with E17 prior to 6/22/2005 */
		ever = XInternAtom(disp, "ENLIGHTENMENT_VERSION", True);
		if (ever == None) {
			/* This is an E without ENLIGHTENMENT_VERSION */
			D(("E16 IPC Protocol not supported"));
			return(None);
		}
	}
	XGetWindowProperty(disp, root, prop, 0, 14, False, AnyPropertyType, &prop2, &format, &num, &after, &str);
	if (str) {
		sscanf((char *) str, "%*s %x", (unsigned int *) &ipc_win);
		XFree(str);
	}
	if (ipc_win != None) {
		if (!XGetGeometry
		    (disp, ipc_win, &dummy_win, &dummy_int, &dummy_int,
		     &dummy_uint, &dummy_uint, &dummy_uint, &dummy_uint)) {
			D((" -> IPC Window property is valid, but the window doesn't exist.\n"));
			ipc_win = None;
		}
		str = NULL;
		if (ipc_win != None) {
			XGetWindowProperty(disp, ipc_win, prop, 0, 14,
					   False, AnyPropertyType, &prop2, &format, &num, &after, &str);
			if (str) {
				XFree(str);
			} else {
				D((" -> IPC Window lacks the proper atom.  I can't talk to fake IPC windows....\n"));
				ipc_win = None;
			}
		}
	}
	if (ipc_win != None) {

		XGetWindowProperty(disp, ipc_win, ever, 0, 14, False,
				   AnyPropertyType, &prop2, &format, &num, &after, &str);
		if (str) {
			/*
			 * This is E17's way of telling us it's only pretending
			 * as a workaround for a bug related to the way java handles
			 * Window Managers.
			 * (Only valid after date of this comment)
			 * -- richlowe 2005-06-22
			 */
			XFree(str);
			D((" -> Found a fake E17 IPC window, ignoring"));
			ipc_win = None;
			e17_fake_ipc = 1;
			return(ipc_win);
		}

		D((" -> IPC Window found and verified as 0x%08x.  Registering feh as an IPC client.\n", (int) ipc_win));
		XSelectInput(disp, ipc_win, StructureNotifyMask | SubstructureNotifyMask);
		enl_ipc_send("set clientname " PACKAGE);
		enl_ipc_send("set version " VERSION);
		enl_ipc_send("set email tom@linuxbrit.co.uk");
		enl_ipc_send("set web http://www.linuxbrit.co.uk");
		enl_ipc_send("set info Feh - be pr0n or be dead");
	}
	if (my_ipc_win == None) {
		my_ipc_win = XCreateSimpleWindow(disp, root, -2, -2, 1, 1, 0, 0, 0);
	}
	return(ipc_win);
}
