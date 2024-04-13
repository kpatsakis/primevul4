static void arcmsr_start_adapter_bgrb(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		arcmsr_hbaA_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		arcmsr_hbaB_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		arcmsr_hbaC_start_bgrb(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		arcmsr_hbaD_start_bgrb(acb);
		break;
	}
}
