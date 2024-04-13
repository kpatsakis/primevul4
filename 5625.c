get_register_name(int num)
{
    if (num == -1)
	return '"';
    else if (num < 10)
	return num + '0';
    else if (num == DELETION_REGISTER)
	return '-';
#ifdef FEAT_CLIPBOARD
    else if (num == STAR_REGISTER)
	return '*';
    else if (num == PLUS_REGISTER)
	return '+';
#endif
    else
    {
#ifdef EBCDIC
	int i;

	// EBCDIC is really braindead ...
	i = 'a' + (num - 10);
	if (i > 'i')
	    i += 7;
	if (i > 'r')
	    i += 8;
	return i;
#else
	return num + 'a' - 10;
#endif
    }
}