void opj_pi_create_encode( 	opj_pi_iterator_t *pi,
							opj_cp_t *cp,
							OPJ_UINT32 tileno,
							OPJ_UINT32 pino,
							OPJ_UINT32 tpnum,
							OPJ_INT32 tppos,
							J2K_T2_MODE t2_mode)
{
	const OPJ_CHAR *prog;
	OPJ_INT32 i;
	OPJ_UINT32 incr_top=1,resetX=0;
	opj_tcp_t *tcps =&cp->tcps[tileno];
	opj_poc_t *tcp= &tcps->pocs[pino];

	prog = opj_j2k_convert_progression_order(tcp->prg);

	pi[pino].first = 1;
	pi[pino].poc.prg = tcp->prg;

    if(!(cp->m_specific_param.m_enc.m_tp_on && ((!OPJ_IS_CINEMA(cp->rsiz) && (t2_mode == FINAL_PASS)) || OPJ_IS_CINEMA(cp->rsiz)))){
		pi[pino].poc.resno0 = tcp->resS;
		pi[pino].poc.resno1 = tcp->resE;
		pi[pino].poc.compno0 = tcp->compS;
		pi[pino].poc.compno1 = tcp->compE;
		pi[pino].poc.layno0 = tcp->layS;
		pi[pino].poc.layno1 = tcp->layE;
		pi[pino].poc.precno0 = tcp->prcS;
		pi[pino].poc.precno1 = tcp->prcE;
		pi[pino].poc.tx0 = (OPJ_INT32)tcp->txS;
		pi[pino].poc.ty0 = (OPJ_INT32)tcp->tyS;
		pi[pino].poc.tx1 = (OPJ_INT32)tcp->txE;
		pi[pino].poc.ty1 = (OPJ_INT32)tcp->tyE;
	}else {
		for(i=tppos+1;i<4;i++){
			switch(prog[i]){
			case 'R':
				pi[pino].poc.resno0 = tcp->resS;
				pi[pino].poc.resno1 = tcp->resE;
				break;
			case 'C':
				pi[pino].poc.compno0 = tcp->compS;
				pi[pino].poc.compno1 = tcp->compE;
				break;
			case 'L':
				pi[pino].poc.layno0 = tcp->layS;
				pi[pino].poc.layno1 = tcp->layE;
				break;
			case 'P':
				switch(tcp->prg){
				case OPJ_LRCP:
				case OPJ_RLCP:
					pi[pino].poc.precno0 = tcp->prcS;
					pi[pino].poc.precno1 = tcp->prcE;
					break;
				default:
					pi[pino].poc.tx0 = (OPJ_INT32)tcp->txS;
					pi[pino].poc.ty0 = (OPJ_INT32)tcp->tyS;
					pi[pino].poc.tx1 = (OPJ_INT32)tcp->txE;
					pi[pino].poc.ty1 = (OPJ_INT32)tcp->tyE;
					break;
				}
				break;
			}
		}

		if(tpnum==0){
			for(i=tppos;i>=0;i--){
				switch(prog[i]){
				case 'C':
					tcp->comp_t = tcp->compS;
					pi[pino].poc.compno0 = tcp->comp_t;
					pi[pino].poc.compno1 = tcp->comp_t+1;
					tcp->comp_t+=1;
					break;
				case 'R':
					tcp->res_t = tcp->resS;
					pi[pino].poc.resno0 = tcp->res_t;
					pi[pino].poc.resno1 = tcp->res_t+1;
					tcp->res_t+=1;
					break;
				case 'L':
					tcp->lay_t = tcp->layS;
					pi[pino].poc.layno0 = tcp->lay_t;
					pi[pino].poc.layno1 = tcp->lay_t+1;
					tcp->lay_t+=1;
					break;
				case 'P':
					switch(tcp->prg){
					case OPJ_LRCP:
					case OPJ_RLCP:
						tcp->prc_t = tcp->prcS;
						pi[pino].poc.precno0 = tcp->prc_t;
						pi[pino].poc.precno1 = tcp->prc_t+1;
						tcp->prc_t+=1;
						break;
					default:
						tcp->tx0_t = tcp->txS;
						tcp->ty0_t = tcp->tyS;
						pi[pino].poc.tx0 = (OPJ_INT32)tcp->tx0_t;
						pi[pino].poc.tx1 = (OPJ_INT32)(tcp->tx0_t + tcp->dx - (tcp->tx0_t % tcp->dx));
						pi[pino].poc.ty0 = (OPJ_INT32)tcp->ty0_t;
						pi[pino].poc.ty1 = (OPJ_INT32)(tcp->ty0_t + tcp->dy - (tcp->ty0_t % tcp->dy));
						tcp->tx0_t = (OPJ_UINT32)pi[pino].poc.tx1;
						tcp->ty0_t = (OPJ_UINT32)pi[pino].poc.ty1;
						break;
					}
					break;
				}
			}
			incr_top=1;
		}else{
			for(i=tppos;i>=0;i--){
				switch(prog[i]){
				case 'C':
					pi[pino].poc.compno0 = tcp->comp_t-1;
					pi[pino].poc.compno1 = tcp->comp_t;
					break;
				case 'R':
					pi[pino].poc.resno0 = tcp->res_t-1;
					pi[pino].poc.resno1 = tcp->res_t;
					break;
				case 'L':
					pi[pino].poc.layno0 = tcp->lay_t-1;
					pi[pino].poc.layno1 = tcp->lay_t;
					break;
				case 'P':
					switch(tcp->prg){
					case OPJ_LRCP:
					case OPJ_RLCP:
						pi[pino].poc.precno0 = tcp->prc_t-1;
						pi[pino].poc.precno1 = tcp->prc_t;
						break;
					default:
						pi[pino].poc.tx0 = (OPJ_INT32)(tcp->tx0_t - tcp->dx - (tcp->tx0_t % tcp->dx));
						pi[pino].poc.tx1 = (OPJ_INT32)tcp->tx0_t ;
						pi[pino].poc.ty0 = (OPJ_INT32)(tcp->ty0_t - tcp->dy - (tcp->ty0_t % tcp->dy));
						pi[pino].poc.ty1 = (OPJ_INT32)tcp->ty0_t ;
						break;
					}
					break;
				}
				if(incr_top==1){
					switch(prog[i]){
					case 'R':
						if(tcp->res_t==tcp->resE){
							if(opj_pi_check_next_level(i-1,cp,tileno,pino,prog)){
								tcp->res_t = tcp->resS;
								pi[pino].poc.resno0 = tcp->res_t;
								pi[pino].poc.resno1 = tcp->res_t+1;
								tcp->res_t+=1;
								incr_top=1;
							}else{
								incr_top=0;
							}
						}else{
							pi[pino].poc.resno0 = tcp->res_t;
							pi[pino].poc.resno1 = tcp->res_t+1;
							tcp->res_t+=1;
							incr_top=0;
						}
						break;
					case 'C':
						if(tcp->comp_t ==tcp->compE){
							if(opj_pi_check_next_level(i-1,cp,tileno,pino,prog)){
								tcp->comp_t = tcp->compS;
								pi[pino].poc.compno0 = tcp->comp_t;
								pi[pino].poc.compno1 = tcp->comp_t+1;
								tcp->comp_t+=1;
								incr_top=1;
							}else{
								incr_top=0;
							}
						}else{
							pi[pino].poc.compno0 = tcp->comp_t;
							pi[pino].poc.compno1 = tcp->comp_t+1;
							tcp->comp_t+=1;
							incr_top=0;
						}
						break;
					case 'L':
						if(tcp->lay_t == tcp->layE){
							if(opj_pi_check_next_level(i-1,cp,tileno,pino,prog)){
								tcp->lay_t = tcp->layS;
								pi[pino].poc.layno0 = tcp->lay_t;
								pi[pino].poc.layno1 = tcp->lay_t+1;
								tcp->lay_t+=1;
								incr_top=1;
							}else{
								incr_top=0;
							}
						}else{
							pi[pino].poc.layno0 = tcp->lay_t;
							pi[pino].poc.layno1 = tcp->lay_t+1;
							tcp->lay_t+=1;
							incr_top=0;
						}
						break;
					case 'P':
						switch(tcp->prg){
						case OPJ_LRCP:
						case OPJ_RLCP:
							if(tcp->prc_t == tcp->prcE){
								if(opj_pi_check_next_level(i-1,cp,tileno,pino,prog)){
									tcp->prc_t = tcp->prcS;
									pi[pino].poc.precno0 = tcp->prc_t;
									pi[pino].poc.precno1 = tcp->prc_t+1;
									tcp->prc_t+=1;
									incr_top=1;
								}else{
									incr_top=0;
								}
							}else{
								pi[pino].poc.precno0 = tcp->prc_t;
								pi[pino].poc.precno1 = tcp->prc_t+1;
								tcp->prc_t+=1;
								incr_top=0;
							}
							break;
						default:
							if(tcp->tx0_t >= tcp->txE){
								if(tcp->ty0_t >= tcp->tyE){
									if(opj_pi_check_next_level(i-1,cp,tileno,pino,prog)){
										tcp->ty0_t = tcp->tyS;
										pi[pino].poc.ty0 = (OPJ_INT32)tcp->ty0_t;
										pi[pino].poc.ty1 = (OPJ_INT32)(tcp->ty0_t + tcp->dy - (tcp->ty0_t % tcp->dy));
										tcp->ty0_t = (OPJ_UINT32)pi[pino].poc.ty1;
										incr_top=1;resetX=1;
									}else{
										incr_top=0;resetX=0;
									}
								}else{
									pi[pino].poc.ty0 = (OPJ_INT32)tcp->ty0_t;
									pi[pino].poc.ty1 = (OPJ_INT32)(tcp->ty0_t + tcp->dy - (tcp->ty0_t % tcp->dy));
									tcp->ty0_t = (OPJ_UINT32)pi[pino].poc.ty1;
									incr_top=0;resetX=1;
								}
								if(resetX==1){
									tcp->tx0_t = tcp->txS;
									pi[pino].poc.tx0 = (OPJ_INT32)tcp->tx0_t;
									pi[pino].poc.tx1 = (OPJ_INT32)(tcp->tx0_t + tcp->dx- (tcp->tx0_t % tcp->dx));
									tcp->tx0_t = (OPJ_UINT32)pi[pino].poc.tx1;
								}
							}else{
								pi[pino].poc.tx0 = (OPJ_INT32)tcp->tx0_t;
								pi[pino].poc.tx1 = (OPJ_INT32)(tcp->tx0_t + tcp->dx- (tcp->tx0_t % tcp->dx));
								tcp->tx0_t = (OPJ_UINT32)pi[pino].poc.tx1;
								incr_top=0;
							}
							break;
						}
						break;
					}
				}
			}
		}
	}
}
