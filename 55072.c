R_API void r_flag_item_set_alias(RFlagItem *item, const char *alias) {
	if (item) {
		free (item->alias);
		item->alias = ISNULLSTR (alias)? NULL: strdup (alias);
	}
}
