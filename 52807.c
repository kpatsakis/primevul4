static struct port_info *find_port_info(struct snd_usb_midi *umidi, int number)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(snd_usbmidi_port_info); ++i) {
		if (snd_usbmidi_port_info[i].id == umidi->usb_id &&
		    snd_usbmidi_port_info[i].port == number)
			return &snd_usbmidi_port_info[i];
	}
	return NULL;
}
