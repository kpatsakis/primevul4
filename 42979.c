file_badseek(struct magic_set *ms)
{
	file_error(ms, errno, "error seeking");
}
