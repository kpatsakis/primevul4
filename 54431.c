void HTML_write(HTStructured * me, const char *s, int l)
{
    const char *p;
    const char *e = s + l;

    if (LYMapsOnly && me->sp[0].tag_number != HTML_OBJECT)
	return;

    for (p = s; p < e; p++)
	HTML_put_character(me, *p);
}
