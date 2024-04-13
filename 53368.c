int tty_hung_up_p(struct file *filp)
{
	return (filp->f_op == &hung_up_tty_fops);
}
