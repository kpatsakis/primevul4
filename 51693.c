static void airspy_video_release(struct v4l2_device *v)
{
	struct airspy *s = container_of(v, struct airspy, v4l2_dev);

	v4l2_ctrl_handler_free(&s->hdl);
	v4l2_device_unregister(&s->v4l2_dev);
	kfree(s);
}
