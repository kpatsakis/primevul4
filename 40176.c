static void filename_to_taskname(char *tcomm, const char *fn, unsigned int len)
{
	int i, ch;

	/* Copies the binary name from after last slash */
	for (i = 0; (ch = *(fn++)) != '\0';) {
		if (ch == '/')
			i = 0; /* overwrite what we wrote */
		else
			if (i < len - 1)
				tcomm[i++] = ch;
	}
	tcomm[i] = '\0';
}
