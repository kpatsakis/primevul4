Strsubstr(Str s, int beg, int len)
{
    Str new_s;
    int i;

    STR_LENGTH_CHECK(s);
    new_s = Strnew();
    if (beg >= s->length)
	return new_s;
    for (i = 0; i < len && beg + i < s->length; i++)
	Strcat_char(new_s, s->ptr[beg + i]);
    return new_s;
}
