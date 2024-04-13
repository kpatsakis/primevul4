static int gtco_input_open(struct input_dev *inputdev)
{
	struct gtco *device = input_get_drvdata(inputdev);

	device->urbinfo->dev = interface_to_usbdev(device->intf);
	if (usb_submit_urb(device->urbinfo, GFP_KERNEL))
		return -EIO;

	return 0;
}
