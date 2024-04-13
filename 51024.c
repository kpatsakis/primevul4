static int arcmsr_abort_one_cmd(struct AdapterControlBlock *acb,
		struct CommandControlBlock *ccb)
{
	int rtn;
	rtn = arcmsr_polling_ccbdone(acb, ccb);
	return rtn;
}
