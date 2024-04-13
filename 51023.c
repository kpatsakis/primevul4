static uint8_t arcmsr_abort_allcmd(struct AdapterControlBlock *acb)
{
	uint8_t rtnval = 0;
	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A: {
		rtnval = arcmsr_hbaA_abort_allcmd(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		rtnval = arcmsr_hbaB_abort_allcmd(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_C: {
		rtnval = arcmsr_hbaC_abort_allcmd(acb);
		}
		break;

	case ACB_ADAPTER_TYPE_D:
		rtnval = arcmsr_hbaD_abort_allcmd(acb);
		break;
	}
	return rtnval;
}
