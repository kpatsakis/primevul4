static void snd_usbmidi_input_data(struct snd_usb_midi_in_endpoint *ep,
				   int portidx, uint8_t *data, int length)
{
	struct usbmidi_in_port *port = &ep->ports[portidx];

	if (!port->substream) {
		dev_dbg(&ep->umidi->dev->dev, "unexpected port %d!\n", portidx);
		return;
	}
	if (!test_bit(port->substream->number, &ep->umidi->input_triggered))
		return;
	snd_rawmidi_receive(port->substream, data, length);
}
