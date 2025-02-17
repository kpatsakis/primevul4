parse_rockridge_NM1(struct file_info *file,
		    const unsigned char *data, int data_length)
{
	if (!file->name_continues)
		archive_string_empty(&file->name);
	file->name_continues = 0;
	if (data_length < 1)
		return;
	/*
	 * NM version 1 extension comprises:
	 *   1 byte flag, value is one of:
	 *     = 0: remainder is name
	 *     = 1: remainder is name, next NM entry continues name
	 *     = 2: "."
	 *     = 4: ".."
	 *     = 32: Implementation specific
	 *     All other values are reserved.
	 */
	switch(data[0]) {
	case 0:
		if (data_length < 2)
			return;
		archive_strncat(&file->name,
		    (const char *)data + 1, data_length - 1);
		break;
	case 1:
		if (data_length < 2)
			return;
		archive_strncat(&file->name,
		    (const char *)data + 1, data_length - 1);
		file->name_continues = 1;
		break;
	case 2:
		archive_strcat(&file->name, ".");
		break;
	case 4:
		archive_strcat(&file->name, "..");
		break;
	default:
		return;
	}

}
