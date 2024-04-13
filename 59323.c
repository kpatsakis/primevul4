static bool may_access_netns(int pid)
{
	int ret;
	char s[200];
	uid_t ruid, suid, euid;
	bool may_access = false;

	ret = getresuid(&ruid, &euid, &suid);
	if (ret) {
		fprintf(stderr, "Failed to get my uids: %s\n", strerror(errno));
		return false;
	}
	ret = setresuid(ruid, ruid, euid);
	if (ret) {
		fprintf(stderr, "Failed to set temp uids to (%d,%d,%d): %s\n",
				(int)ruid, (int)ruid, (int)euid, strerror(errno));
		return false;
	}
	ret = snprintf(s, 200, "/proc/%d/ns/net", pid);
	if (ret < 0 || ret >= 200)  // can't happen
		return false;
	ret = access(s, R_OK);
	if (ret) {
		fprintf(stderr, "Uid %d may not access %s: %s\n",
				(int)ruid, s, strerror(errno));
	}
	may_access = ret == 0;
	ret = setresuid(ruid, euid, suid);
	if (ret) {
		fprintf(stderr, "Failed to restore uids to (%d,%d,%d): %s\n",
				(int)ruid, (int)euid, (int)suid, strerror(errno));
		may_access = false;
	}
	return may_access;
}
