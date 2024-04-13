static int sysctl_check_table(const char *path, struct ctl_table *table)
{
	int err = 0;
	for (; table->procname; table++) {
		if (table->child)
			err = sysctl_err(path, table, "Not a file");

		if ((table->proc_handler == proc_dostring) ||
		    (table->proc_handler == proc_dointvec) ||
		    (table->proc_handler == proc_dointvec_minmax) ||
		    (table->proc_handler == proc_dointvec_jiffies) ||
		    (table->proc_handler == proc_dointvec_userhz_jiffies) ||
		    (table->proc_handler == proc_dointvec_ms_jiffies) ||
		    (table->proc_handler == proc_doulongvec_minmax) ||
		    (table->proc_handler == proc_doulongvec_ms_jiffies_minmax)) {
			if (!table->data)
				err = sysctl_err(path, table, "No data");
			if (!table->maxlen)
				err = sysctl_err(path, table, "No maxlen");
		}
		if (!table->proc_handler)
			err = sysctl_err(path, table, "No proc_handler");

		if ((table->mode & (S_IRUGO|S_IWUGO)) != table->mode)
			err = sysctl_err(path, table, "bogus .mode 0%o",
				table->mode);
	}
	return err;
}
