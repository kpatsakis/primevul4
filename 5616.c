write_reg_contents(
    int		name,
    char_u	*str,
    int		maxlen,
    int		must_append)
{
    write_reg_contents_ex(name, str, maxlen, must_append, MAUTO, 0L);
}