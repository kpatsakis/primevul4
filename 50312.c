Stralign_center(Str s, int width)
{
    Str n;
    int i, w;

    STR_LENGTH_CHECK(s);
    if (s->length >= width)
	return Strdup(s);
    n = Strnew_size(width);
    w = (width - s->length) / 2;
    for (i = 0; i < w; i++)
	Strcat_char(n, ' ');
    Strcat(n, s);
    for (i = w + s->length; i < width; i++)
	Strcat_char(n, ' ');
    return n;
}
