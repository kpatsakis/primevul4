static int show_path_component_truncated(FILE *out, const char *name, int len)
{
	int cnt;
	for (cnt = 0; cnt < len; cnt++) {
		int ch = name[cnt];
		if (!ch || ch == '\n')
			return -1;
		fputc(ch, out);
	}
	return len;
}
