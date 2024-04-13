static void option_date_format(const char *fmt)
{
	if (!strcmp(fmt, "raw"))
		whenspec = WHENSPEC_RAW;
	else if (!strcmp(fmt, "rfc2822"))
		whenspec = WHENSPEC_RFC2822;
	else if (!strcmp(fmt, "now"))
		whenspec = WHENSPEC_NOW;
	else
		die("unknown --date-format argument %s", fmt);
}
