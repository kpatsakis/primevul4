static void snd_msndmidi_input_drop(struct snd_msndmidi *mpu)
{
	u16 tail;

	tail = readw(mpu->dev->MIDQ + JQS_wTail);
	writew(tail, mpu->dev->MIDQ + JQS_wHead);
}
