static int substream_open(struct snd_rawmidi_substream *substream, int dir,
			  int open)
{
	struct snd_usb_midi *umidi = substream->rmidi->private_data;
	struct snd_kcontrol *ctl;

	down_read(&umidi->disc_rwsem);
	if (umidi->disconnected) {
		up_read(&umidi->disc_rwsem);
		return open ? -ENODEV : 0;
	}

	mutex_lock(&umidi->mutex);
	if (open) {
		if (!umidi->opened[0] && !umidi->opened[1]) {
			if (umidi->roland_load_ctl) {
				ctl = umidi->roland_load_ctl;
				ctl->vd[0].access |=
					SNDRV_CTL_ELEM_ACCESS_INACTIVE;
				snd_ctl_notify(umidi->card,
				       SNDRV_CTL_EVENT_MASK_INFO, &ctl->id);
				update_roland_altsetting(umidi);
			}
		}
		umidi->opened[dir]++;
		if (umidi->opened[1])
			snd_usbmidi_input_start(&umidi->list);
	} else {
		umidi->opened[dir]--;
		if (!umidi->opened[1])
			snd_usbmidi_input_stop(&umidi->list);
		if (!umidi->opened[0] && !umidi->opened[1]) {
			if (umidi->roland_load_ctl) {
				ctl = umidi->roland_load_ctl;
				ctl->vd[0].access &=
					~SNDRV_CTL_ELEM_ACCESS_INACTIVE;
				snd_ctl_notify(umidi->card,
				       SNDRV_CTL_EVENT_MASK_INFO, &ctl->id);
			}
		}
	}
	mutex_unlock(&umidi->mutex);
	up_read(&umidi->disc_rwsem);
	return 0;
}
