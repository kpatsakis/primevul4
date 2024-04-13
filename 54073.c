static ssize_t snd_compr_read(struct file *f, char __user *buf,
		size_t count, loff_t *offset)
{
	struct snd_compr_file *data = f->private_data;
	struct snd_compr_stream *stream;
	size_t avail;
	int retval;

	if (snd_BUG_ON(!data))
		return -EFAULT;

	stream = &data->stream;
	mutex_lock(&stream->device->lock);

	/* read is allowed when stream is running, paused, draining and setup
	 * (yes setup is state which we transition to after stop, so if user
	 * wants to read data after stop we allow that)
	 */
	switch (stream->runtime->state) {
	case SNDRV_PCM_STATE_OPEN:
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_XRUN:
	case SNDRV_PCM_STATE_SUSPENDED:
	case SNDRV_PCM_STATE_DISCONNECTED:
		retval = -EBADFD;
		goto out;
	}

	avail = snd_compr_get_avail(stream);
	pr_debug("avail returned %ld\n", (unsigned long)avail);
	/* calculate how much we can read from buffer */
	if (avail > count)
		avail = count;

	if (stream->ops->copy) {
		retval = stream->ops->copy(stream, buf, avail);
	} else {
		retval = -ENXIO;
		goto out;
	}
	if (retval > 0)
		stream->runtime->total_bytes_transferred += retval;

out:
	mutex_unlock(&stream->device->lock);
	return retval;
}
