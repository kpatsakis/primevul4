get_lifetime(uint32_t v)
{
	static char buf[20];

	if (v == (uint32_t)~0UL)
		return "infinity";
	else {
		snprintf(buf, sizeof(buf), "%us", v);
		return buf;
	}
}
