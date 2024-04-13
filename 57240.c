static int snd_msndmidi_input_close(struct snd_rawmidi_substream *substream)
{
	struct snd_msndmidi *mpu;

	mpu = substream->rmidi->private_data;
	snd_msnd_send_dsp_cmd(mpu->dev, HDEX_MIDI_IN_STOP);
	clear_bit(MSNDMIDI_MODE_BIT_INPUT, &mpu->mode);
	mpu->substream_input = NULL;
	snd_msnd_disable_irq(mpu->dev);
	return 0;
}
