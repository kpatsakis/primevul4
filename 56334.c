smb_errstr(int class, int num)
{
    static char ret[128];
    int i, j;

    ret[0] = 0;

    for (i = 0; err_classes[i].class; i++)
	if (err_classes[i].code == class) {
	    if (err_classes[i].err_msgs) {
		const err_code_struct *err = err_classes[i].err_msgs;
		for (j = 0; err[j].name; j++)
		    if (num == err[j].code) {
			snprintf(ret, sizeof(ret), "%s - %s (%s)",
			    err_classes[i].class, err[j].name, err[j].message);
			return ret;
		    }
	    }

	    snprintf(ret, sizeof(ret), "%s - %d", err_classes[i].class, num);
	    return ret;
	}

    snprintf(ret, sizeof(ret), "ERROR: Unknown error (%d,%d)", class, num);
    return(ret);
}
