 static int uas_configure_endpoints(struct uas_dev_info *devinfo)
{
	struct usb_host_endpoint *eps[4] = { };
	struct usb_device *udev = devinfo->udev;
	int r;

	r = uas_find_endpoints(devinfo->intf->cur_altsetting, eps);
	if (r)
		return r;

	devinfo->cmd_pipe = usb_sndbulkpipe(udev,
					    usb_endpoint_num(&eps[0]->desc));
	devinfo->status_pipe = usb_rcvbulkpipe(udev,
					    usb_endpoint_num(&eps[1]->desc));
	devinfo->data_in_pipe = usb_rcvbulkpipe(udev,
					    usb_endpoint_num(&eps[2]->desc));
	devinfo->data_out_pipe = usb_sndbulkpipe(udev,
					    usb_endpoint_num(&eps[3]->desc));

	if (udev->speed < USB_SPEED_SUPER) {
		devinfo->qdepth = 32;
		devinfo->use_streams = 0;
	} else {
		devinfo->qdepth = usb_alloc_streams(devinfo->intf, eps + 1,
						    3, MAX_CMNDS, GFP_NOIO);
		if (devinfo->qdepth < 0)
			return devinfo->qdepth;
		devinfo->use_streams = 1;
	}

	return 0;
}
