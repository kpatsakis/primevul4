static int acm_reset_resume(struct usb_interface *intf)
{
	struct acm *acm = usb_get_intfdata(intf);

	if (test_bit(ASYNCB_INITIALIZED, &acm->port.flags))
		tty_port_tty_hangup(&acm->port, false);

	return acm_resume(intf);
}
