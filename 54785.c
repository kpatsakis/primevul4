static int unlink1(struct usbtest_dev *dev, int pipe, int size, int async)
{
	struct urb		*urb;
	struct completion	completion;
	int			retval = 0;

	init_completion(&completion);
	urb = simple_alloc_urb(testdev_to_usbdev(dev), pipe, size, 0);
	if (!urb)
		return -ENOMEM;
	urb->context = &completion;
	urb->complete = unlink1_callback;

	if (usb_pipeout(urb->pipe)) {
		simple_fill_buf(urb);
		urb->transfer_flags |= URB_ZERO_PACKET;
	}

	/* keep the endpoint busy.  there are lots of hc/hcd-internal
	 * states, and testing should get to all of them over time.
	 *
	 * FIXME want additional tests for when endpoint is STALLing
	 * due to errors, or is just NAKing requests.
	 */
	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval != 0) {
		dev_err(&dev->intf->dev, "submit fail %d\n", retval);
		return retval;
	}

	/* unlinking that should always work.  variable delay tests more
	 * hcd states and code paths, even with little other system load.
	 */
	msleep(jiffies % (2 * INTERRUPT_RATE));
	if (async) {
		while (!completion_done(&completion)) {
			retval = usb_unlink_urb(urb);

			if (retval == 0 && usb_pipein(urb->pipe))
				retval = simple_check_buf(dev, urb);

			switch (retval) {
			case -EBUSY:
			case -EIDRM:
				/* we can't unlink urbs while they're completing
				 * or if they've completed, and we haven't
				 * resubmitted. "normal" drivers would prevent
				 * resubmission, but since we're testing unlink
				 * paths, we can't.
				 */
				ERROR(dev, "unlink retry\n");
				continue;
			case 0:
			case -EINPROGRESS:
				break;

			default:
				dev_err(&dev->intf->dev,
					"unlink fail %d\n", retval);
				return retval;
			}

			break;
		}
	} else
		usb_kill_urb(urb);

	wait_for_completion(&completion);
	retval = urb->status;
	simple_free_urb(urb);

	if (async)
		return (retval == -ECONNRESET) ? 0 : retval - 1000;
	else
		return (retval == -ENOENT || retval == -EPERM) ?
				0 : retval - 2000;
}
