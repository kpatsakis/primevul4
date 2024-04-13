R_API void r_flag_item_free(RFlagItem *item) {
	if (item) {
		free (item->color);
		free (item->comment);
		free (item->alias);
		/* release only one of the two pointers if they are the same */
		if (item->name != item->realname) {
			free (item->name);
		}
		free (item->realname);
		free (item);
	}
}
