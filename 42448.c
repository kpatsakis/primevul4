proc_do_sync_ports(ctl_table *table, int write,
		   void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int *valp = table->data;
	int val = *valp;
	int rc;

	rc = proc_dointvec(table, write, buffer, lenp, ppos);
	if (write && (*valp != val)) {
		if (*valp < 1 || !is_power_of_2(*valp)) {
			/* Restore the correct value */
			*valp = val;
		}
	}
	return rc;
}
