static void arcmsr_stop_adapter_bgrb(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		arcmsr_hbaA_stop_bgrb(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		arcmsr_hbaB_stop_bgrb(acb);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		arcmsr_hbaC_stop_bgrb(acb);
		}
		break;
	case ACB_ADAPTER_TYPE_D:
		arcmsr_hbaD_stop_bgrb(acb);
		break;
	}
}
