static void arcmsr_flush_adapter_cache(struct AdapterControlBlock *acb)
{
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		arcmsr_hbaA_flush_cache(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		arcmsr_hbaB_flush_cache(acb);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		arcmsr_hbaC_flush_cache(acb);
		}
		break;
	case ACB_ADAPTER_TYPE_D:
		arcmsr_hbaD_flush_cache(acb);
		break;
	}
}
