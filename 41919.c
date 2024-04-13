void console_stop(struct console *console)
{
	console_lock();
	console->flags &= ~CON_ENABLED;
	console_unlock();
}
