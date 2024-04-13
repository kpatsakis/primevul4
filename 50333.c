Strremovefirstspaces(Str s)
{
    int i;

    STR_LENGTH_CHECK(s);
    for (i = 0; i < s->length && IS_SPACE(s->ptr[i]); i++) ;
    if (i == 0)
	return;
    Strdelete(s, 0, i);
}
