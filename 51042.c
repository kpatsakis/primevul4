static bool arcmsr_get_firmware_spec(struct AdapterControlBlock *acb)
{
	bool rtn = false;

	switch (acb->adapter_type) {
	case ACB_ADAPTER_TYPE_A:
		rtn = arcmsr_hbaA_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_B:
		rtn = arcmsr_hbaB_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_C:
		rtn = arcmsr_hbaC_get_config(acb);
		break;
	case ACB_ADAPTER_TYPE_D:
		rtn = arcmsr_hbaD_get_config(acb);
		break;
	default:
		break;
	}
	if (acb->firm_numbers_queue > ARCMSR_MAX_OUTSTANDING_CMD)
		acb->maxOutstanding = ARCMSR_MAX_OUTSTANDING_CMD;
	else
		acb->maxOutstanding = acb->firm_numbers_queue - 1;
	acb->host->can_queue = acb->maxOutstanding;
	return rtn;
}
