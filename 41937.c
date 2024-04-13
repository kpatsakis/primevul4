static int __init printk_late_init(void)
{
	struct console *con;

	for_each_console(con) {
		if (!keep_bootcon && con->flags & CON_BOOT) {
			printk(KERN_INFO "turn off boot console %s%d\n",
				con->name, con->index);
			unregister_console(con);
		}
	}
	hotcpu_notifier(console_cpu_notify, 0);
	return 0;
}
