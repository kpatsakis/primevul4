signed char feh_wm_get_wm_is_e(void)
{
	static signed char e = -1;

	/* check if E is actually running */
	if (e == -1) {
		/* XXX: This only covers E17 prior to 6/22/05 */
		if ((XInternAtom(disp, "ENLIGHTENMENT_COMMS", True) != None)
		    && (XInternAtom(disp, "ENLIGHTENMENT_VERSION", True) != None)) {
			D(("Enlightenment detected.\n"));
			e = 1;
		} else {
			D(("Enlightenment not detected.\n"));
			e = 0;
		}
	}
	return(e);
}
