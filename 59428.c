lha_replace_path_separator(struct lha *lha, struct archive_entry *entry)
{
	const wchar_t *wp;
	size_t i;

	if ((wp = archive_entry_pathname_w(entry)) != NULL) {
		archive_wstrcpy(&(lha->ws), wp);
		for (i = 0; i < archive_strlen(&(lha->ws)); i++) {
			if (lha->ws.s[i] == L'\\')
				lha->ws.s[i] = L'/';
		}
		archive_entry_copy_pathname_w(entry, lha->ws.s);
	}

	if ((wp = archive_entry_symlink_w(entry)) != NULL) {
		archive_wstrcpy(&(lha->ws), wp);
		for (i = 0; i < archive_strlen(&(lha->ws)); i++) {
			if (lha->ws.s[i] == L'\\')
				lha->ws.s[i] = L'/';
		}
		archive_entry_copy_symlink_w(entry, lha->ws.s);
	}
}
