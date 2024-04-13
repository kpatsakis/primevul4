static int simple_io(
	struct usbtest_dev	*tdev,
	struct urb		*urb,
	int			iterations,
	int			vary,
	int			expected,
	const char		*label
)
{
	struct usb_device	*udev = urb->dev;
	int			max = urb->transfer_buffer_length;
	struct completion	completion;
	int			retval = 0;
	unsigned long		expire;

	urb->context = &completion;
	while (retval == 0 && iterations-- > 0) {
		init_completion(&completion);
		if (usb_pipeout(urb->pipe)) {
			simple_fill_buf(urb);
			urb->transfer_flags |= URB_ZERO_PACKET;
		}
		retval = usb_submit_urb(urb, GFP_KERNEL);
		if (retval != 0)
			break;

		expire = msecs_to_jiffies(SIMPLE_IO_TIMEOUT);
		if (!wait_for_completion_timeout(&completion, expire)) {
			usb_kill_urb(urb);
			retval = (urb->status == -ENOENT ?
				  -ETIMEDOUT : urb->status);
		} else {
			retval = urb->status;
		}

		urb->dev = udev;
		if (retval == 0 && usb_pipein(urb->pipe))
			retval = simple_check_buf(tdev, urb);

		if (vary) {
			int	len = urb->transfer_buffer_length;

			len += vary;
			len %= max;
			if (len == 0)
				len = (vary < max) ? vary : max;
			urb->transfer_buffer_length = len;
		}

		/* FIXME if endpoint halted, clear halt (and log) */
	}
	urb->transfer_buffer_length = max;

	if (expected != retval)
		dev_err(&udev->dev,
			"%s failed, iterations left %d, status %d (not %d)\n",
				label, iterations, retval, expected);
	return retval;
}
