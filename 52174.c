get_system_info(void)
    {
    FILE* fp;
    static char buf[1000];

    fp = popen("uname -a", "r");
    if (fp == NULL)
	return NULL;
    fgets(buf, sizeof(buf), fp);
    pclose(fp);
    return buf;
    }
