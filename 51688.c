static int airspy_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct airspy *s = vb2_get_drv_priv(vq);
	int ret;

	dev_dbg(s->dev, "\n");

	if (!s->udev)
		return -ENODEV;

	mutex_lock(&s->v4l2_lock);

	s->sequence = 0;

	set_bit(POWER_ON, &s->flags);

	ret = airspy_alloc_stream_bufs(s);
	if (ret)
		goto err_clear_bit;

	ret = airspy_alloc_urbs(s);
	if (ret)
		goto err_free_stream_bufs;

	ret = airspy_submit_urbs(s);
	if (ret)
		goto err_free_urbs;

	/* start hardware streaming */
	ret = airspy_ctrl_msg(s, CMD_RECEIVER_MODE, 1, 0, NULL, 0);
	if (ret)
		goto err_kill_urbs;

	goto exit_mutex_unlock;

err_kill_urbs:
	airspy_kill_urbs(s);
err_free_urbs:
	airspy_free_urbs(s);
err_free_stream_bufs:
	airspy_free_stream_bufs(s);
err_clear_bit:
	clear_bit(POWER_ON, &s->flags);

	/* return all queued buffers to vb2 */
	{
		struct airspy_frame_buf *buf, *tmp;

		list_for_each_entry_safe(buf, tmp, &s->queued_bufs, list) {
			list_del(&buf->list);
			vb2_buffer_done(&buf->vb.vb2_buf,
					VB2_BUF_STATE_QUEUED);
		}
	}

exit_mutex_unlock:
	mutex_unlock(&s->v4l2_lock);

	return ret;
}
