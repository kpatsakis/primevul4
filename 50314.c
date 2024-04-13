Stralign_right(Str s, int width)
{
    Str n;
    int i;

    STR_LENGTH_CHECK(s);
    if (s->length >= width)
	return Strdup(s);
    n = Strnew_size(width);
    for (i = s->length; i < width; i++)
	Strcat_char(n, ' ');
    Strcat(n, s);
    return n;
}
