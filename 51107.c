static void arcmsr_request_device_map(unsigned long pacb)
{
	struct AdapterControlBlock *acb = (struct AdapterControlBlock *)pacb;
	switch (acb->adapter_type) {
		case ACB_ADAPTER_TYPE_A: {
			arcmsr_hbaA_request_device_map(acb);
		}
		break;
		case ACB_ADAPTER_TYPE_B: {
			arcmsr_hbaB_request_device_map(acb);
		}
		break;
		case ACB_ADAPTER_TYPE_C: {
			arcmsr_hbaC_request_device_map(acb);
		}
		break;
		case ACB_ADAPTER_TYPE_D:
			arcmsr_hbaD_request_device_map(acb);
		break;
	}
}
