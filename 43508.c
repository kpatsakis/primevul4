static void liSleep(LARGE_INTEGER *li, int timeout)
{
	HANDLE hTimer;
	DWORD ret;

	if (unlikely(timeout <= 0))
		return;

	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (unlikely(!hTimer))
		quit(1, "Failed to create hTimer in liSleep");
	ret = SetWaitableTimer(hTimer, li, 0, NULL, NULL, 0);
	if (unlikely(!ret))
		quit(1, "Failed to SetWaitableTimer in liSleep");
	/* We still use a timeout as a sanity check in case the system time
	 * is changed while we're running */
	ret = WaitForSingleObject(hTimer, timeout);
	if (unlikely(ret != WAIT_OBJECT_0 && ret != WAIT_TIMEOUT))
		quit(1, "Failed to WaitForSingleObject in liSleep");
	CloseHandle(hTimer);
}
