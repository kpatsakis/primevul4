int snd_msndmidi_new(struct snd_card *card, int device)
{
	struct snd_msnd *chip = card->private_data;
	struct snd_msndmidi *mpu;
	struct snd_rawmidi *rmidi;
	int err;

	err = snd_rawmidi_new(card, "MSND-MIDI", device, 1, 1, &rmidi);
	if (err < 0)
		return err;
	mpu = kzalloc(sizeof(*mpu), GFP_KERNEL);
	if (mpu == NULL) {
		snd_device_free(card, rmidi);
		return -ENOMEM;
	}
	mpu->dev = chip;
	chip->msndmidi_mpu = mpu;
	rmidi->private_data = mpu;
	rmidi->private_free = snd_msndmidi_free;
	spin_lock_init(&mpu->input_lock);
	strcpy(rmidi->name, "MSND MIDI");
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_INPUT,
			    &snd_msndmidi_input);
	rmidi->info_flags |= SNDRV_RAWMIDI_INFO_INPUT;
	return 0;
}
