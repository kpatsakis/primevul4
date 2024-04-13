static int lg_ultrax_remote_mapping(struct hid_input *hi,
		struct hid_usage *usage, unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) != HID_UP_LOGIVENDOR)
		return 0;

	set_bit(EV_REP, hi->input->evbit);
	switch (usage->hid & HID_USAGE) {
	/* Reported on Logitech Ultra X Media Remote */
	case 0x004: lg_map_key_clear(KEY_AGAIN);	break;
	case 0x00d: lg_map_key_clear(KEY_HOME);		break;
	case 0x024: lg_map_key_clear(KEY_SHUFFLE);	break;
	case 0x025: lg_map_key_clear(KEY_TV);		break;
	case 0x026: lg_map_key_clear(KEY_MENU);		break;
	case 0x031: lg_map_key_clear(KEY_AUDIO);	break;
	case 0x032: lg_map_key_clear(KEY_TEXT);		break;
	case 0x033: lg_map_key_clear(KEY_LAST);		break;
	case 0x047: lg_map_key_clear(KEY_MP3);		break;
	case 0x048: lg_map_key_clear(KEY_DVD);		break;
	case 0x049: lg_map_key_clear(KEY_MEDIA);	break;
	case 0x04a: lg_map_key_clear(KEY_VIDEO);	break;
	case 0x04b: lg_map_key_clear(KEY_ANGLE);	break;
	case 0x04c: lg_map_key_clear(KEY_LANGUAGE);	break;
	case 0x04d: lg_map_key_clear(KEY_SUBTITLE);	break;
	case 0x051: lg_map_key_clear(KEY_RED);		break;
	case 0x052: lg_map_key_clear(KEY_CLOSE);	break;

	default:
		return 0;
	}
	return 1;
}
