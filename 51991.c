static void check_reset_of_active_ep(struct usb_device *udev,
		unsigned int epnum, char *ioctl_name)
{
	struct usb_host_endpoint **eps;
	struct usb_host_endpoint *ep;

	eps = (epnum & USB_DIR_IN) ? udev->ep_in : udev->ep_out;
	ep = eps[epnum & 0x0f];
	if (ep && !list_empty(&ep->urb_list))
		dev_warn(&udev->dev, "Process %d (%s) called USBDEVFS_%s for active endpoint 0x%02x\n",
				task_pid_nr(current), current->comm,
				ioctl_name, epnum);
}
