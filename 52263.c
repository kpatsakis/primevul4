free_CodersInfo(struct _7z_coders_info *ci)
{
	unsigned i;

	if (ci->folders) {
		for (i = 0; i < ci->numFolders; i++)
			free_Folder(&(ci->folders[i]));
		free(ci->folders);
	}
}
