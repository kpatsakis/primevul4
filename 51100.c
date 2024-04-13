static int arcmsr_polling_ccbdone(struct AdapterControlBlock *acb,
					struct CommandControlBlock *poll_ccb)
{
	int rtn = 0;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		rtn = arcmsr_hbaA_polling_ccbdone(acb, poll_ccb);
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		rtn = arcmsr_hbaB_polling_ccbdone(acb, poll_ccb);
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		rtn = arcmsr_hbaC_polling_ccbdone(acb, poll_ccb);
		}
		break;
	case ACB_ADAPTER_TYPE_D:
		rtn = arcmsr_hbaD_polling_ccbdone(acb, poll_ccb);
		break;
	}
	return rtn;
}
