SYSCALL_DEFINE1(ssetmask, int, newmask)
{
	int old = current->blocked.sig[0];
	sigset_t newset;

	siginitset(&newset, newmask);
	set_current_blocked(&newset);

	return old;
}
