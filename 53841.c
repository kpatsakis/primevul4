static void aiptek_close(struct input_dev *inputdev)
{
	struct aiptek *aiptek = input_get_drvdata(inputdev);

	usb_kill_urb(aiptek->urb);
}
