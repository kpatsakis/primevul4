static void generic_done(int result)
{
	command_status = result;
	cont = &wakeup_cont;
}
