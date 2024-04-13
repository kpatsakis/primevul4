static int snd_msnd_mpu401_open(struct snd_mpu401 *mpu)
{
	snd_msnd_enable_irq(mpu->private_data);
	snd_msnd_send_dsp_cmd(mpu->private_data, HDEX_MIDI_IN_START);
	return 0;
}
