static unsigned int snd_compr_poll(struct file *f, poll_table *wait)
{
	struct snd_compr_file *data = f->private_data;
	struct snd_compr_stream *stream;
	size_t avail;
	int retval = 0;

	if (snd_BUG_ON(!data))
		return -EFAULT;
	stream = &data->stream;
	if (snd_BUG_ON(!stream))
		return -EFAULT;

	mutex_lock(&stream->device->lock);
	if (stream->runtime->state == SNDRV_PCM_STATE_OPEN) {
		retval = -EBADFD;
		goto out;
	}
	poll_wait(f, &stream->runtime->sleep, wait);

	avail = snd_compr_get_avail(stream);
	pr_debug("avail is %ld\n", (unsigned long)avail);
	/* check if we have at least one fragment to fill */
	switch (stream->runtime->state) {
	case SNDRV_PCM_STATE_DRAINING:
		/* stream has been woken up after drain is complete
		 * draining done so set stream state to stopped
		 */
		retval = snd_compr_get_poll(stream);
		stream->runtime->state = SNDRV_PCM_STATE_SETUP;
		break;
	case SNDRV_PCM_STATE_RUNNING:
	case SNDRV_PCM_STATE_PREPARED:
	case SNDRV_PCM_STATE_PAUSED:
		if (avail >= stream->runtime->fragment_size)
			retval = snd_compr_get_poll(stream);
		break;
	default:
		if (stream->direction == SND_COMPRESS_PLAYBACK)
			retval = POLLOUT | POLLWRNORM | POLLERR;
		else
			retval = POLLIN | POLLRDNORM | POLLERR;
		break;
	}
out:
	mutex_unlock(&stream->device->lock);
	return retval;
}
