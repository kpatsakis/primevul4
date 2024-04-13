Strcat_charp(Str x, const char *y)
{
    if (y == NULL)
	return;
    Strcat_charp_n(x, y, strlen(y));
}
