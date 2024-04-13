set_last_csearch(int c, char_u *s UNUSED, int len UNUSED)
{
    *lastc = c;
    lastc_bytelen = len;
    if (len)
	memcpy(lastc_bytes, s, len);
    else
	CLEAR_FIELD(lastc_bytes);
}