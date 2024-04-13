Strremovetrailingspaces(Str s)
{
    int i;

    STR_LENGTH_CHECK(s);
    for (i = s->length - 1; i >= 0 && IS_SPACE(s->ptr[i]); i--) ;
    s->length = i + 1;
    s->ptr[i + 1] = '\0';
}
