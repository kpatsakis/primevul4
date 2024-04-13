static int finish_port_resume(struct usb_device *udev)
{
	int	status = 0;
	u16	devstatus = 0;

	/* caller owns the udev device lock */
	dev_dbg(&udev->dev, "%s\n",
		udev->reset_resume ? "finish reset-resume" : "finish resume");

	/* usb ch9 identifies four variants of SUSPENDED, based on what
	 * state the device resumes to.  Linux currently won't see the
	 * first two on the host side; they'd be inside hub_port_init()
	 * during many timeouts, but hub_wq can't suspend until later.
	 */
	usb_set_device_state(udev, udev->actconfig
			? USB_STATE_CONFIGURED
			: USB_STATE_ADDRESS);

	/* 10.5.4.5 says not to reset a suspended port if the attached
	 * device is enabled for remote wakeup.  Hence the reset
	 * operation is carried out here, after the port has been
	 * resumed.
	 */
	if (udev->reset_resume) {
		/*
		 * If the device morphs or switches modes when it is reset,
		 * we don't want to perform a reset-resume.  We'll fail the
		 * resume, which will cause a logical disconnect, and then
		 * the device will be rediscovered.
		 */
 retry_reset_resume:
		if (udev->quirks & USB_QUIRK_RESET)
			status = -ENODEV;
		else
			status = usb_reset_and_verify_device(udev);
	}

	/* 10.5.4.5 says be sure devices in the tree are still there.
	 * For now let's assume the device didn't go crazy on resume,
	 * and device drivers will know about any resume quirks.
	 */
	if (status == 0) {
		devstatus = 0;
		status = usb_get_status(udev, USB_RECIP_DEVICE, 0, &devstatus);

		/* If a normal resume failed, try doing a reset-resume */
		if (status && !udev->reset_resume && udev->persist_enabled) {
			dev_dbg(&udev->dev, "retry with reset-resume\n");
			udev->reset_resume = 1;
			goto retry_reset_resume;
		}
	}

	if (status) {
		dev_dbg(&udev->dev, "gone after usb resume? status %d\n",
				status);
	/*
	 * There are a few quirky devices which violate the standard
	 * by claiming to have remote wakeup enabled after a reset,
	 * which crash if the feature is cleared, hence check for
	 * udev->reset_resume
	 */
	} else if (udev->actconfig && !udev->reset_resume) {
		if (udev->speed < USB_SPEED_SUPER) {
			if (devstatus & (1 << USB_DEVICE_REMOTE_WAKEUP))
				status = usb_disable_remote_wakeup(udev);
		} else {
			status = usb_get_status(udev, USB_RECIP_INTERFACE, 0,
					&devstatus);
			if (!status && devstatus & (USB_INTRF_STAT_FUNC_RW_CAP
					| USB_INTRF_STAT_FUNC_RW))
				status = usb_disable_remote_wakeup(udev);
		}

		if (status)
			dev_dbg(&udev->dev,
				"disable remote wakeup, status %d\n",
				status);
		status = 0;
	}
	return status;
}
