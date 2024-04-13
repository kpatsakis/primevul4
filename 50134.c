snd_pcm_sframes_t snd_pcm_lib_write(struct snd_pcm_substream *substream, const void __user *buf, snd_pcm_uframes_t size)
{
	struct snd_pcm_runtime *runtime;
	int nonblock;
	int err;

	err = pcm_sanity_check(substream);
	if (err < 0)
		return err;
	runtime = substream->runtime;
	nonblock = !!(substream->f_flags & O_NONBLOCK);

	if (runtime->access != SNDRV_PCM_ACCESS_RW_INTERLEAVED &&
	    runtime->channels > 1)
		return -EINVAL;
	return snd_pcm_lib_write1(substream, (unsigned long)buf, size, nonblock,
				  snd_pcm_lib_write_transfer);
}
