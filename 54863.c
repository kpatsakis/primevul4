void usb_serial_console_init(int minor)
{
	if (minor == 0) {
		/*
		 * Call register_console() if this is the first device plugged
		 * in.  If we call it earlier, then the callback to
		 * console_setup() will fail, as there is not a device seen by
		 * the USB subsystem yet.
		 */
		/*
		 * Register console.
		 * NOTES:
		 * console_setup() is called (back) immediately (from
		 * register_console). console_write() is called immediately
		 * from register_console iff CON_PRINTBUFFER is set in flags.
		 */
		pr_debug("registering the USB serial console.\n");
		register_console(&usbcons);
	}
}
