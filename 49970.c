static int bsg_complete_all_commands(struct bsg_device *bd)
{
	struct bsg_command *bc;
	int ret, tret;

	dprintk("%s: entered\n", bd->name);

	/*
	 * wait for all commands to complete
	 */
	io_wait_event(bd->wq_done, bsg_complete(bd));

	/*
	 * discard done commands
	 */
	ret = 0;
	do {
		spin_lock_irq(&bd->lock);
		if (!bd->queued_cmds) {
			spin_unlock_irq(&bd->lock);
			break;
		}
		spin_unlock_irq(&bd->lock);

		bc = bsg_get_done_cmd(bd);
		if (IS_ERR(bc))
			break;

		tret = blk_complete_sgv4_hdr_rq(bc->rq, &bc->hdr, bc->bio,
						bc->bidi_bio);
		if (!ret)
			ret = tret;

		bsg_free_command(bc);
	} while (1);

	return ret;
}
