void resume_console(void)
{
	if (!console_suspend_enabled)
		return;
	down(&console_sem);
	console_suspended = 0;
	console_unlock();
}
