static int acm_tty_ioctl(struct tty_struct *tty,
					unsigned int cmd, unsigned long arg)
{
	struct acm *acm = tty->driver_data;
	int rv = -ENOIOCTLCMD;

	switch (cmd) {
	case TIOCGSERIAL: /* gets serial port data */
		rv = get_serial_info(acm, (struct serial_struct __user *) arg);
		break;
	case TIOCSSERIAL:
		rv = set_serial_info(acm, (struct serial_struct __user *) arg);
		break;
	case TIOCMIWAIT:
		rv = usb_autopm_get_interface(acm->control);
		if (rv < 0) {
			rv = -EIO;
			break;
		}
		rv = wait_serial_change(acm, arg);
		usb_autopm_put_interface(acm->control);
		break;
	case TIOCGICOUNT:
		rv = get_serial_usage(acm, (struct serial_icounter_struct __user *) arg);
		break;
	}

	return rv;
}
