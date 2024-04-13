_kdc_find_padata(const KDC_REQ *req, int *start, int type)
{
    if (req->padata == NULL)
	return NULL;

    while((size_t)*start < req->padata->len){
	(*start)++;
	if(req->padata->val[*start - 1].padata_type == (unsigned)type)
	    return &req->padata->val[*start - 1];
    }
    return NULL;
}
