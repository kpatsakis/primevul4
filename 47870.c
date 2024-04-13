static char *get_cpuset(const char *cg)
{
	char *answer;

	if (!cgfs_get_value("cpuset", cg, "cpuset.cpus", &answer))
		return NULL;
	return answer;
}
