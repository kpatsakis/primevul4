int_unix_date(uint32_t dos_date)
{
    struct tm t;

    if (dos_date == 0)
	return(0);

    interpret_dos_date(dos_date, &t);
    t.tm_wday = 1;
    t.tm_yday = 1;
    t.tm_isdst = 0;

    return (mktime(&t));
}
