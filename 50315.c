Strcat(Str x, Str y)
{
    STR_LENGTH_CHECK(y);
    Strcat_charp_n(x, y->ptr, y->length);
}
