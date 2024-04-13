numstr(int x)
{
	static char buf[20];

	snprintf(buf, sizeof(buf), "%#x", x);
	return buf;
}
