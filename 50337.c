Strtruncate(Str s, int pos)
{
    STR_LENGTH_CHECK(s);
    s->ptr[pos] = '\0';
    s->length = pos;
}
