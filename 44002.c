int_option(str, valp)
    char *str;
    int *valp;
{
    u_int32_t v;

    if (!number_option(str, &v, 0))
	return 0;
    *valp = (int) v;
    return 1;
}
