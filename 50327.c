Strinsert_char(Str s, int pos, char c)
{
    int i;
    STR_LENGTH_CHECK(s);
    if (pos < 0 || s->length < pos)
	return;
    if (s->length + 2 > s->area_size)
	Strgrow(s);
    for (i = s->length; i > pos; i--)
	s->ptr[i] = s->ptr[i - 1];
    s->ptr[++s->length] = '\0';
    s->ptr[pos] = c;
}
