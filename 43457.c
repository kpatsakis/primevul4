void RenameThread(const char* name)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "cg@%s", name);
#if defined(PR_SET_NAME)
	prctl(PR_SET_NAME, buf, 0, 0, 0);
#elif (defined(__FreeBSD__) || defined(__OpenBSD__))
	pthread_set_name_np(pthread_self(), buf);
#elif defined(MAC_OSX)
	pthread_setname_np(buf);
#else
	(void)buf;
#endif
}
