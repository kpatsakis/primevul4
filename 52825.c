static void snd_usbmidi_emagic_init_out(struct snd_usb_midi_out_endpoint *ep)
{
	static const u8 init_data[] = {
		/* initialization magic: "get version" */
		0xf0,
		0x00, 0x20, 0x31,	/* Emagic */
		0x64,			/* Unitor8 */
		0x0b,			/* version number request */
		0x00,			/* command version */
		0x00,			/* EEPROM, box 0 */
		0xf7
	};
	send_bulk_static_data(ep, init_data, sizeof(init_data));
	/* while we're at it, pour on more magic */
	send_bulk_static_data(ep, init_data, sizeof(init_data));
}
