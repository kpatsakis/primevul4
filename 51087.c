static irqreturn_t arcmsr_interrupt(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		return arcmsr_hbaA_handle_isr(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		return arcmsr_hbaB_handle_isr(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		return arcmsr_hbaC_handle_isr(acb);
	case ACB_ADAPTER_TYPE_D:
		return arcmsr_hbaD_handle_isr(acb);
	default:
		return IRQ_NONE;
	}
}
