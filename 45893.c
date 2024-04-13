cmsUInt32Number mywcslen(const wchar_t *s)
{
    const wchar_t *p;

    p = s;
    while (*p)
        p++;

    return (cmsUInt32Number)(p - s);
}
