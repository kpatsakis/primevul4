static void snd_usbmidi_maudio_broken_running_status_input(
					struct snd_usb_midi_in_endpoint *ep,
					uint8_t *buffer, int buffer_length)
{
	int i;

	for (i = 0; i + 3 < buffer_length; i += 4)
		if (buffer[i] != 0) {
			int cable = buffer[i] >> 4;
			u8 cin = buffer[i] & 0x0f;
			struct usbmidi_in_port *port = &ep->ports[cable];
			int length;

			length = snd_usbmidi_cin_length[cin];
			if (cin == 0xf && buffer[i + 1] >= 0xf8)
				; /* realtime msg: no running status change */
			else if (cin >= 0x8 && cin <= 0xe)
				/* channel msg */
				port->running_status_length = length - 1;
			else if (cin == 0x4 &&
				 port->running_status_length != 0 &&
				 buffer[i + 1] < 0x80)
				/* CIN 4 that is not a SysEx */
				length = port->running_status_length;
			else
				/*
				 * All other msgs cannot begin running status.
				 * (A channel msg sent as two or three CIN 0xF
				 * packets could in theory, but this device
				 * doesn't use this format.)
				 */
				port->running_status_length = 0;
			snd_usbmidi_input_data(ep, cable, &buffer[i + 1],
					       length);
		}
}
