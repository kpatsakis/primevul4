Strdelete(Str s, int pos, int n)
{
    int i;
    STR_LENGTH_CHECK(s);
    if (s->length <= pos + n) {
	s->ptr[pos] = '\0';
	s->length = pos;
	return;
    }
    for (i = pos; i < s->length - n; i++)
	s->ptr[i] = s->ptr[i + n];
    s->ptr[i] = '\0';
    s->length = i;
}
