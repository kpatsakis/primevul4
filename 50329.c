Strlower(Str s)
{
    int i;
    STR_LENGTH_CHECK(s);
    for (i = 0; i < s->length; i++)
	s->ptr[i] = TOLOWER(s->ptr[i]);
}
