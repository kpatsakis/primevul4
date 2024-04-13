static inline int snd_compr_get_poll(struct snd_compr_stream *stream)
{
	if (stream->direction == SND_COMPRESS_PLAYBACK)
		return POLLOUT | POLLWRNORM;
	else
		return POLLIN | POLLRDNORM;
}
