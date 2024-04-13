static int snd_compr_stop(struct snd_compr_stream *stream)
{
	int retval;

	if (stream->runtime->state == SNDRV_PCM_STATE_PREPARED ||
			stream->runtime->state == SNDRV_PCM_STATE_SETUP)
		return -EPERM;
	retval = stream->ops->trigger(stream, SNDRV_PCM_TRIGGER_STOP);
	if (!retval) {
		stream->runtime->state = SNDRV_PCM_STATE_SETUP;
		wake_up(&stream->runtime->sleep);
		stream->runtime->hw_pointer = 0;
		stream->runtime->app_pointer = 0;
		stream->runtime->total_bytes_available = 0;
		stream->runtime->total_bytes_transferred = 0;
	}
	return retval;
}
