R_API void r_flag_item_set_comment(RFlagItem *item, const char *comment) {
	if (item) {
		free (item->comment);
		item->comment = ISNULLSTR (comment) ? NULL : strdup (comment);
	}
}
