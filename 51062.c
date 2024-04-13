static void arcmsr_hbaB_request_device_map(struct AdapterControlBlock *acb)
{
	struct MessageUnit_B *reg = acb->pmuB;
	if (unlikely(atomic_read(&acb->rq_map_token) == 0) || ((acb->acb_flags & ACB_F_BUS_RESET) != 0 ) || ((acb->acb_flags & ACB_F_ABORT) != 0 )){
		mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
		return;
	} else {
		acb->fw_flag = FW_NORMAL;
		if (atomic_read(&acb->ante_token_value) == atomic_read(&acb->rq_map_token)) {
			atomic_set(&acb->rq_map_token, 16);
		}
		atomic_set(&acb->ante_token_value, atomic_read(&acb->rq_map_token));
		if (atomic_dec_and_test(&acb->rq_map_token)) {
			mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
			return;
		}
		writel(ARCMSR_MESSAGE_GET_CONFIG, reg->drv2iop_doorbell);
		mod_timer(&acb->eternal_timer, jiffies + msecs_to_jiffies(6 * HZ));
	}
	return;
}
