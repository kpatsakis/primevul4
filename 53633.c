static void usb_set_lpm_mel(struct usb_device *udev,
		struct usb3_lpm_parameters *udev_lpm_params,
		unsigned int udev_exit_latency,
		struct usb_hub *hub,
		struct usb3_lpm_parameters *hub_lpm_params,
		unsigned int hub_exit_latency)
{
	unsigned int total_mel;
	unsigned int device_mel;
	unsigned int hub_mel;

	/*
	 * Calculate the time it takes to transition all links from the roothub
	 * to the parent hub into U0.  The parent hub must then decode the
	 * packet (hub header decode latency) to figure out which port it was
	 * bound for.
	 *
	 * The Hub Header decode latency is expressed in 0.1us intervals (0x1
	 * means 0.1us).  Multiply that by 100 to get nanoseconds.
	 */
	total_mel = hub_lpm_params->mel +
		(hub->descriptor->u.ss.bHubHdrDecLat * 100);

	/*
	 * How long will it take to transition the downstream hub's port into
	 * U0?  The greater of either the hub exit latency or the device exit
	 * latency.
	 *
	 * The BOS U1/U2 exit latencies are expressed in 1us intervals.
	 * Multiply that by 1000 to get nanoseconds.
	 */
	device_mel = udev_exit_latency * 1000;
	hub_mel = hub_exit_latency * 1000;
	if (device_mel > hub_mel)
		total_mel += device_mel;
	else
		total_mel += hub_mel;

	udev_lpm_params->mel = total_mel;
}
