static int set_procfs_val(const char *path, const char *val)
{
	int rc = -1;
	FILE *fp = fopen(path, "w");

	if (fp) {
		if (fprintf(fp, "%s", val) > 0)
			rc = 0;
		fclose(fp);
	}
	return rc;
}