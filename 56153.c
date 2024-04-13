bittok2str(register const struct tok *lp, register const char *fmt,
	   register u_int v)
{
    return (bittok2str_internal(lp, fmt, v, ", "));
}
