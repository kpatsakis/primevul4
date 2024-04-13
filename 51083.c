static void arcmsr_hbaD_request_device_map(struct AdapterControlBlock *acb)
{
	struct MessageUnit_D *reg = acb->pmuD;

	if (unlikely(atomic_read(&acb->rq_map_token) == 0) ||
		((acb->acb_flags & ACB_F_BUS_RESET) != 0) ||
		((acb->acb_flags & ACB_F_ABORT) != 0)) {
		mod_timer(&acb->eternal_timer,
			jiffies + msecs_to_jiffies(6 * HZ));
	} else {
		acb->fw_flag = FW_NORMAL;
		if (atomic_read(&acb->ante_token_value) ==
			atomic_read(&acb->rq_map_token)) {
			atomic_set(&acb->rq_map_token, 16);
		}
		atomic_set(&acb->ante_token_value,
			atomic_read(&acb->rq_map_token));
		if (atomic_dec_and_test(&acb->rq_map_token)) {
			mod_timer(&acb->eternal_timer, jiffies +
				msecs_to_jiffies(6 * HZ));
			return;
		}
		writel(ARCMSR_INBOUND_MESG0_GET_CONFIG,
			reg->inbound_msgaddr0);
		mod_timer(&acb->eternal_timer, jiffies +
			msecs_to_jiffies(6 * HZ));
	}
}
