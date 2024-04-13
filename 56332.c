nt_errstr(uint32_t err)
{
    static char ret[128];
    int i;

    ret[0] = 0;

    for (i = 0; nt_errors[i].name; i++) {
	if (err == nt_errors[i].code)
	    return nt_errors[i].name;
    }

    snprintf(ret, sizeof(ret), "0x%08x", err);
    return ret;
}
