static void snd_usbmidi_out_endpoint_delete(struct snd_usb_midi_out_endpoint *ep)
{
	snd_usbmidi_out_endpoint_clear(ep);
	kfree(ep);
}
