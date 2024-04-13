snd_pcm_sframes_t snd_pcm_lib_writev(struct snd_pcm_substream *substream,
				     void __user **bufs,
				     snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime;
	int nonblock;
	int err;

	err = pcm_sanity_check(substream);
	if (err < 0)
		return err;
	runtime = substream->runtime;
	nonblock = !!(substream->f_flags & O_NONBLOCK);

	if (runtime->access != SNDRV_PCM_ACCESS_RW_NONINTERLEAVED)
		return -EINVAL;
	return snd_pcm_lib_write1(substream, (unsigned long)bufs, frames,
				  nonblock, snd_pcm_lib_writev_transfer);
}
