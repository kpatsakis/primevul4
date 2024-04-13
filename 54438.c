static char *MakeNewTitle(STRING2PTR value, int src_type)
{
    char *ptr;
    char *newtitle = NULL;

    StrAllocCopy(newtitle, "[");
    if (value != 0 && value[src_type] != 0) {
	ptr = strrchr(value[src_type], '/');
	if (!ptr) {
	    StrAllocCat(newtitle, value[src_type]);
	} else {
	    StrAllocCat(newtitle, ptr + 1);
	}
    } else {
	ptr = 0;
    }
#ifdef SH_EX			/* 1998/04/02 (Thu) 16:02:00 */

    /* for proxy server 1998/12/19 (Sat) 11:53:30 */
    if (AS_casecomp(newtitle + 1, "internal-gopher-menu") == 0) {
	StrAllocCopy(newtitle, "+");
    } else if (AS_casecomp(newtitle + 1, "internal-gopher-unknown") == 0) {
	StrAllocCopy(newtitle, " ");
    } else {
	/* normal title */
	ptr = strrchr(newtitle, '.');
	if (ptr) {
	    if (AS_casecomp(ptr, ".gif") == 0)
		*ptr = '\0';
	    else if (AS_casecomp(ptr, ".jpg") == 0)
		*ptr = '\0';
	    else if (AS_casecomp(ptr, ".jpeg") == 0)
		*ptr = '\0';
	}
	StrAllocCat(newtitle, "]");
    }
#else
    StrAllocCat(newtitle, "]");
#endif
    return newtitle;
}
