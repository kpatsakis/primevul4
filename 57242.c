static int snd_msndmidi_input_open(struct snd_rawmidi_substream *substream)
{
	struct snd_msndmidi *mpu;

	snd_printdd("snd_msndmidi_input_open()\n");

	mpu = substream->rmidi->private_data;

	mpu->substream_input = substream;

	snd_msnd_enable_irq(mpu->dev);

	snd_msnd_send_dsp_cmd(mpu->dev, HDEX_MIDI_IN_START);
	set_bit(MSNDMIDI_MODE_BIT_INPUT, &mpu->mode);
	return 0;
}
