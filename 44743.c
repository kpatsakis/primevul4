static void print_result(char *message, int inr)
{
	int i;

	DPRINT("%s ", message);
	if (inr >= 0)
		for (i = 0; i < inr; i++)
			pr_cont("repl[%d]=%x ", i, reply_buffer[i]);
	pr_cont("\n");
}
