static int test_unaligned_bulk(
	struct usbtest_dev *tdev,
	int pipe,
	unsigned length,
	int iterations,
	unsigned transfer_flags,
	const char *label)
{
	int retval;
	struct urb *urb = usbtest_alloc_urb(testdev_to_usbdev(tdev),
			pipe, length, transfer_flags, 1, 0, simple_callback);

	if (!urb)
		return -ENOMEM;

	retval = simple_io(tdev, urb, iterations, 0, 0, label);
	simple_free_urb(urb);
	return retval;
}
