static void snd_msnd_mpu401_close(struct snd_mpu401 *mpu)
{
	snd_msnd_send_dsp_cmd(mpu->private_data, HDEX_MIDI_IN_STOP);
	snd_msnd_disable_irq(mpu->private_data);
}
