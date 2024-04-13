static int set_name(RFlagItem *item, const char *name) {
	if (item->name != item->realname) {
		free (item->name);
	}
	item->name = strdup (name);
	if (!item->name) {
		return false;
	}
	r_str_chop (item->name);
	r_name_filter (item->name, 0); // TODO: name_filter should be chopping already
	free (item->realname);
	item->realname = item->name;
	return true;
}
