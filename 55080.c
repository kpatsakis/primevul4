R_API RFlagItem *r_flag_set_next(RFlag *f, const char *name, ut64 off, ut32 size) {
	if (!r_flag_get (f, name)) {
		return r_flag_set (f, name, off, size);
	}
	int i, newNameSize = strlen (name);
	char *newName = malloc (newNameSize + 16);
	strcpy (newName, name);
	for (i = 0; ; i++) {
		snprintf (newName + newNameSize, 15, ".%d", i);
		if (!r_flag_get (f, newName)) {
			RFlagItem *fi = r_flag_set (f, newName, off, size);
			free (newName);
			return fi;
		}
	}
	return NULL;
}
