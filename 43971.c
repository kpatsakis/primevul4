static int lg_wireless_mapping(struct hid_input *hi, struct hid_usage *usage,
		unsigned long **bit, int *max)
{
	if ((usage->hid & HID_USAGE_PAGE) != HID_UP_CONSUMER)
		return 0;

	switch (usage->hid & HID_USAGE) {
	case 0x1001: lg_map_key_clear(KEY_MESSENGER);		break;
	case 0x1003: lg_map_key_clear(KEY_SOUND);		break;
	case 0x1004: lg_map_key_clear(KEY_VIDEO);		break;
	case 0x1005: lg_map_key_clear(KEY_AUDIO);		break;
	case 0x100a: lg_map_key_clear(KEY_DOCUMENTS);		break;
	/* The following two entries are Playlist 1 and 2 on the MX3200 */
	case 0x100f: lg_map_key_clear(KEY_FN_1);		break;
	case 0x1010: lg_map_key_clear(KEY_FN_2);		break;
	case 0x1011: lg_map_key_clear(KEY_PREVIOUSSONG);	break;
	case 0x1012: lg_map_key_clear(KEY_NEXTSONG);		break;
	case 0x1013: lg_map_key_clear(KEY_CAMERA);		break;
	case 0x1014: lg_map_key_clear(KEY_MESSENGER);		break;
	case 0x1015: lg_map_key_clear(KEY_RECORD);		break;
	case 0x1016: lg_map_key_clear(KEY_PLAYER);		break;
	case 0x1017: lg_map_key_clear(KEY_EJECTCD);		break;
	case 0x1018: lg_map_key_clear(KEY_MEDIA);		break;
	case 0x1019: lg_map_key_clear(KEY_PROG1);		break;
	case 0x101a: lg_map_key_clear(KEY_PROG2);		break;
	case 0x101b: lg_map_key_clear(KEY_PROG3);		break;
	case 0x101c: lg_map_key_clear(KEY_CYCLEWINDOWS);	break;
	case 0x101f: lg_map_key_clear(KEY_ZOOMIN);		break;
	case 0x1020: lg_map_key_clear(KEY_ZOOMOUT);		break;
	case 0x1021: lg_map_key_clear(KEY_ZOOMRESET);		break;
	case 0x1023: lg_map_key_clear(KEY_CLOSE);		break;
	case 0x1027: lg_map_key_clear(KEY_MENU);		break;
	/* this one is marked as 'Rotate' */
	case 0x1028: lg_map_key_clear(KEY_ANGLE);		break;
	case 0x1029: lg_map_key_clear(KEY_SHUFFLE);		break;
	case 0x102a: lg_map_key_clear(KEY_BACK);		break;
	case 0x102b: lg_map_key_clear(KEY_CYCLEWINDOWS);	break;
	case 0x102d: lg_map_key_clear(KEY_WWW);			break;
	/* The following two are 'Start/answer call' and 'End/reject call'
	   on the MX3200 */
	case 0x1031: lg_map_key_clear(KEY_OK);			break;
	case 0x1032: lg_map_key_clear(KEY_CANCEL);		break;
	case 0x1041: lg_map_key_clear(KEY_BATTERY);		break;
	case 0x1042: lg_map_key_clear(KEY_WORDPROCESSOR);	break;
	case 0x1043: lg_map_key_clear(KEY_SPREADSHEET);		break;
	case 0x1044: lg_map_key_clear(KEY_PRESENTATION);	break;
	case 0x1045: lg_map_key_clear(KEY_UNDO);		break;
	case 0x1046: lg_map_key_clear(KEY_REDO);		break;
	case 0x1047: lg_map_key_clear(KEY_PRINT);		break;
	case 0x1048: lg_map_key_clear(KEY_SAVE);		break;
	case 0x1049: lg_map_key_clear(KEY_PROG1);		break;
	case 0x104a: lg_map_key_clear(KEY_PROG2);		break;
	case 0x104b: lg_map_key_clear(KEY_PROG3);		break;
	case 0x104c: lg_map_key_clear(KEY_PROG4);		break;

	default:
		return 0;
	}
	return 1;
}
