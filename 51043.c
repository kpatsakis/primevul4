static struct QBUFFER __iomem *arcmsr_get_iop_wqbuffer(struct AdapterControlBlock *acb)
{
	struct QBUFFER __iomem *pqbuffer = NULL;
	switch (acb->adapter_type) {

	case ACB_ADAPTER_TYPE_A: {
		struct MessageUnit_A __iomem *reg = acb->pmuA;
		pqbuffer = (struct QBUFFER __iomem *) &reg->message_wbuffer;
		}
		break;

	case ACB_ADAPTER_TYPE_B: {
		struct MessageUnit_B  *reg = acb->pmuB;
		pqbuffer = (struct QBUFFER __iomem *)reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_C: {
		struct MessageUnit_C __iomem *reg = acb->pmuC;
		pqbuffer = (struct QBUFFER __iomem *)&reg->message_wbuffer;
		}
		break;
	case ACB_ADAPTER_TYPE_D: {
		struct MessageUnit_D *reg = acb->pmuD;
		pqbuffer = (struct QBUFFER __iomem *)reg->message_wbuffer;
		}
		break;
	}
	return pqbuffer;
}
