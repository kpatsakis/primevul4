static void main_command_interrupt(void)
{
	cancel_delayed_work(&fd_timer);
	cont->interrupt();
}
