static void snd_msndmidi_free(struct snd_rawmidi *rmidi)
{
	struct snd_msndmidi *mpu = rmidi->private_data;
	kfree(mpu);
}
