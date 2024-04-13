static void snd_usbmidi_emagic_finish_out(struct snd_usb_midi_out_endpoint *ep)
{
	static const u8 finish_data[] = {
		/* switch to patch mode with last preset */
		0xf0,
		0x00, 0x20, 0x31,	/* Emagic */
		0x64,			/* Unitor8 */
		0x10,			/* patch switch command */
		0x00,			/* command version */
		0x7f,			/* to all boxes */
		0x40,			/* last preset in EEPROM */
		0xf7
	};
	send_bulk_static_data(ep, finish_data, sizeof(finish_data));
}
