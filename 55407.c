EXPORTED uint32_t mboxlist_string_to_mbtype(const char *string)
{
    uint32_t mbtype = 0;

    if (!string) return 0; /* null just means default */

    for (; *string; string++) {
        switch (*string) {
        case 'a':
            mbtype |= MBTYPE_ADDRESSBOOK;
            break;
        case 'b':
            mbtype |= MBTYPE_COLLECTION;
            break;
        case 'c':
            mbtype |= MBTYPE_CALENDAR;
            break;
        case 'd':
            mbtype |= MBTYPE_DELETED;
            break;
        case 'm':
            mbtype |= MBTYPE_MOVING;
            break;
        case 'n':
            mbtype |= MBTYPE_NETNEWS;
            break;
        case 'r':
            mbtype |= MBTYPE_REMOTE;
            break;
        case 'z':
            mbtype |= MBTYPE_RESERVE;
            break;
        }
    }

    return mbtype;
}
