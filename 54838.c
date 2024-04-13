static void snd_usb_stream_disconnect(struct snd_usb_stream *as)
{
	int idx;
	struct snd_usb_substream *subs;

	for (idx = 0; idx < 2; idx++) {
		subs = &as->substream[idx];
		if (!subs->num_formats)
			continue;
		subs->interface = -1;
		subs->data_endpoint = NULL;
		subs->sync_endpoint = NULL;
	}
}
