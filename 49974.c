static struct bsg_command *bsg_get_done_cmd(struct bsg_device *bd)
{
	struct bsg_command *bc;
	int ret;

	do {
		bc = bsg_next_done_cmd(bd);
		if (bc)
			break;

		if (!test_bit(BSG_F_BLOCK, &bd->flags)) {
			bc = ERR_PTR(-EAGAIN);
			break;
		}

		ret = wait_event_interruptible(bd->wq_done, bd->done_cmds);
		if (ret) {
			bc = ERR_PTR(-ERESTARTSYS);
			break;
		}
	} while (1);

	dprintk("%s: returning done %p\n", bd->name, bc);

	return bc;
}
