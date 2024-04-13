Strfgetall(FILE * f)
{
    Str s = Strnew();
    int c;
    while ((c = fgetc(f)) != EOF) {
	Strcat_char(s, c);
    }
    return s;
}
