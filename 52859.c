static void snd_usbmidi_output_trigger(struct snd_rawmidi_substream *substream,
				       int up)
{
	struct usbmidi_out_port *port =
		(struct usbmidi_out_port *)substream->runtime->private_data;

	port->active = up;
	if (up) {
		if (port->ep->umidi->disconnected) {
			/* gobble up remaining bytes to prevent wait in
			 * snd_rawmidi_drain_output */
			while (!snd_rawmidi_transmit_empty(substream))
				snd_rawmidi_transmit_ack(substream, 1);
			return;
		}
		tasklet_schedule(&port->ep->tasklet);
	}
}
