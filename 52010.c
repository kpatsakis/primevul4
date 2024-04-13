static int proc_claim_port(struct usb_dev_state *ps, void __user *arg)
{
	unsigned portnum;
	int rc;

	if (get_user(portnum, (unsigned __user *) arg))
		return -EFAULT;
	rc = usb_hub_claim_port(ps->dev, portnum, ps);
	if (rc == 0)
		snoop(&ps->dev->dev, "port %d claimed by process %d: %s\n",
			portnum, task_pid_nr(current), current->comm);
	return rc;
}
