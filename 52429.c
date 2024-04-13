static int ims_pcu_switch_to_bootloader(struct ims_pcu *pcu)
{
	int error;

	/* Execute jump to the bootoloader */
	error = ims_pcu_execute_command(pcu, JUMP_TO_BTLDR, NULL, 0);
	if (error) {
		dev_err(pcu->dev,
			"Failure when sending JUMP TO BOOLTLOADER command, error: %d\n",
			error);
		return error;
	}

	return 0;
}
