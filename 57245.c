static void set_default_audio_parameters(struct snd_msnd *chip)
{
	chip->play_sample_size = DEFSAMPLESIZE;
	chip->play_sample_rate = DEFSAMPLERATE;
	chip->play_channels = DEFCHANNELS;
	chip->capture_sample_size = DEFSAMPLESIZE;
	chip->capture_sample_rate = DEFSAMPLERATE;
	chip->capture_channels = DEFCHANNELS;
}
