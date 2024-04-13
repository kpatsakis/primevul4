Strfgets(FILE * f)
{
    Str s = Strnew();
    int c;
    while ((c = fgetc(f)) != EOF) {
	Strcat_char(s, c);
	if (c == '\n')
	    break;
    }
    return s;
}
