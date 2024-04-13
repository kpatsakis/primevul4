static void parse_mark(void)
{
	const char *v;
	if (skip_prefix(command_buf.buf, "mark :", &v)) {
		next_mark = strtoumax(v, NULL, 10);
		read_next_command();
	}
	else
		next_mark = 0;
}
