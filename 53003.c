static void gtco_input_close(struct input_dev *inputdev)
{
	struct gtco *device = input_get_drvdata(inputdev);

	usb_kill_urb(device->urbinfo);
}
