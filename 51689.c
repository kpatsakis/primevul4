static void airspy_stop_streaming(struct vb2_queue *vq)
{
	struct airspy *s = vb2_get_drv_priv(vq);

	dev_dbg(s->dev, "\n");

	mutex_lock(&s->v4l2_lock);

	/* stop hardware streaming */
	airspy_ctrl_msg(s, CMD_RECEIVER_MODE, 0, 0, NULL, 0);

	airspy_kill_urbs(s);
	airspy_free_urbs(s);
	airspy_free_stream_bufs(s);

	airspy_cleanup_queued_bufs(s);

	clear_bit(POWER_ON, &s->flags);

	mutex_unlock(&s->v4l2_lock);
}
