static void arcmsr_iop_parking(struct AdapterControlBlock *acb)
{
	if (acb) {
		/* stop adapter background rebuild */
		if (acb->acb_flags & ACB_F_MSG_START_BGRB) {
			uint32_t intmask_org;
			acb->acb_flags &= ~ACB_F_MSG_START_BGRB;
			intmask_org = arcmsr_disable_outbound_ints(acb);
			arcmsr_stop_adapter_bgrb(acb);
			arcmsr_flush_adapter_cache(acb);
			arcmsr_enable_outbound_ints(acb, intmask_org);
		}
	}
}
