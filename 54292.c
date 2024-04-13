static int snd_compr_drain(struct snd_compr_stream *stream)
{
	int retval;

	if (stream->runtime->state == SNDRV_PCM_STATE_PREPARED ||
			stream->runtime->state == SNDRV_PCM_STATE_SETUP)
		return -EPERM;
	retval = stream->ops->trigger(stream, SND_COMPR_TRIGGER_DRAIN);
	if (!retval) {
		stream->runtime->state = SNDRV_PCM_STATE_DRAINING;
		wake_up(&stream->runtime->sleep);
	}
	return retval;
}
