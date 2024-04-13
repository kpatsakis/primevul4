static int hidp_input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
	struct hidp_session *session = input_get_drvdata(dev);

	return hidp_queue_event(session, dev, type, code, value);
}
