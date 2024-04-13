int snd_pcm_hw_rule_noresample(struct snd_pcm_runtime *runtime,
			       unsigned int base_rate)
{
	return snd_pcm_hw_rule_add(runtime, SNDRV_PCM_HW_PARAMS_NORESAMPLE,
				   SNDRV_PCM_HW_PARAM_RATE,
				   snd_pcm_hw_rule_noresample_func,
				   (void *)(uintptr_t)base_rate,
				   SNDRV_PCM_HW_PARAM_RATE, -1);
}
