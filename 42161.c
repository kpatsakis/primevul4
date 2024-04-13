static int hidp_hidinput_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
	struct hid_device *hid = input_get_drvdata(dev);
	struct hidp_session *session = hid->driver_data;

	return hidp_queue_event(session, dev, type, code, value);
}
