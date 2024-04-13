static void airspy_disconnect(struct usb_interface *intf)
{
	struct v4l2_device *v = usb_get_intfdata(intf);
	struct airspy *s = container_of(v, struct airspy, v4l2_dev);

	dev_dbg(s->dev, "\n");

	mutex_lock(&s->vb_queue_lock);
	mutex_lock(&s->v4l2_lock);
	/* No need to keep the urbs around after disconnection */
	s->udev = NULL;
	v4l2_device_disconnect(&s->v4l2_dev);
	video_unregister_device(&s->vdev);
	mutex_unlock(&s->v4l2_lock);
	mutex_unlock(&s->vb_queue_lock);

	v4l2_device_put(&s->v4l2_dev);
}
