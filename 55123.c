void kvmppc_claim_lpid(long lpid)
{
	set_bit(lpid, lpid_inuse);
}
