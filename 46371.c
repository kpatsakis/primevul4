monitor_child_handler(int sig)
{
	kill(monitor_child_pid, sig);
}
