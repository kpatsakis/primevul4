num_entries(char *bufstart, char *end_of_buf, char **lastentry, size_t size)
{
	int len;
	unsigned int entrycount = 0;
	unsigned int next_offset = 0;
	FILE_DIRECTORY_INFO *entryptr;

	if (bufstart == NULL)
		return 0;

	entryptr = (FILE_DIRECTORY_INFO *)bufstart;

	while (1) {
		entryptr = (FILE_DIRECTORY_INFO *)
					((char *)entryptr + next_offset);

		if ((char *)entryptr + size > end_of_buf) {
			cifs_dbg(VFS, "malformed search entry would overflow\n");
			break;
		}

		len = le32_to_cpu(entryptr->FileNameLength);
		if ((char *)entryptr + len + size > end_of_buf) {
			cifs_dbg(VFS, "directory entry name would overflow frame end of buf %p\n",
				 end_of_buf);
			break;
		}

		*lastentry = (char *)entryptr;
		entrycount++;

		next_offset = le32_to_cpu(entryptr->NextEntryOffset);
		if (!next_offset)
			break;
	}

	return entrycount;
}
