Strshrink(Str s, int n)
{
    STR_LENGTH_CHECK(s);
    if (n >= s->length) {
	s->length = 0;
	s->ptr[0] = '\0';
    }
    else {
	s->length -= n;
	s->ptr[s->length] = '\0';
    }
}
