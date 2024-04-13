void opj_pi_update_encoding_parameters(	const opj_image_t *p_image,
                                        opj_cp_t *p_cp,
                                        OPJ_UINT32 p_tile_no )
{
	/* encoding parameters to set */
	OPJ_UINT32 l_max_res;
	OPJ_UINT32 l_max_prec;
	OPJ_INT32 l_tx0,l_tx1,l_ty0,l_ty1;
	OPJ_UINT32 l_dx_min,l_dy_min;

	/* pointers */
	opj_tcp_t *l_tcp = 00;

	/* preconditions */
	assert(p_cp != 00);
	assert(p_image != 00);
	assert(p_tile_no < p_cp->tw * p_cp->th);

	l_tcp = &(p_cp->tcps[p_tile_no]);

	/* get encoding parameters */
	opj_get_encoding_parameters(p_image,p_cp,p_tile_no,&l_tx0,&l_tx1,&l_ty0,&l_ty1,&l_dx_min,&l_dy_min,&l_max_prec,&l_max_res);

	if (l_tcp->POC) {
		opj_pi_update_encode_poc_and_final(p_cp,p_tile_no,l_tx0,l_tx1,l_ty0,l_ty1,l_max_prec,l_max_res,l_dx_min,l_dy_min);
	}
	else {
		opj_pi_update_encode_not_poc(p_cp,p_image->numcomps,p_tile_no,l_tx0,l_tx1,l_ty0,l_ty1,l_max_prec,l_max_res,l_dx_min,l_dy_min);
	}
}
