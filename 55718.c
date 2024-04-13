OPJ_BOOL opj_tcd_decode_tile(opj_tcd_t *p_tcd,
                             OPJ_BYTE *p_src,
                             OPJ_UINT32 p_max_length,
                             OPJ_UINT32 p_tile_no,
                             opj_codestream_index_t *p_cstr_index,
                             opj_event_mgr_t *p_manager
                            )
{
    OPJ_UINT32 l_data_read;
    p_tcd->tcd_tileno = p_tile_no;
    p_tcd->tcp = &(p_tcd->cp->tcps[p_tile_no]);

#ifdef TODO_MSD /* FIXME */
    /* INDEX >>  */
    if (p_cstr_info) {
        OPJ_UINT32 resno, compno, numprec = 0;
        for (compno = 0; compno < (OPJ_UINT32) p_cstr_info->numcomps; compno++) {
            opj_tcp_t *tcp = &p_tcd->cp->tcps[0];
            opj_tccp_t *tccp = &tcp->tccps[compno];
            opj_tcd_tilecomp_t *tilec_idx = &p_tcd->tcd_image->tiles->comps[compno];
            for (resno = 0; resno < tilec_idx->numresolutions; resno++) {
                opj_tcd_resolution_t *res_idx = &tilec_idx->resolutions[resno];
                p_cstr_info->tile[p_tile_no].pw[resno] = res_idx->pw;
                p_cstr_info->tile[p_tile_no].ph[resno] = res_idx->ph;
                numprec += res_idx->pw * res_idx->ph;
                p_cstr_info->tile[p_tile_no].pdx[resno] = tccp->prcw[resno];
                p_cstr_info->tile[p_tile_no].pdy[resno] = tccp->prch[resno];
            }
        }
        p_cstr_info->tile[p_tile_no].packet = (opj_packet_info_t *) opj_malloc(
                p_cstr_info->numlayers * numprec * sizeof(opj_packet_info_t));
        p_cstr_info->packno = 0;
    }
    /* << INDEX */
#endif

    /*--------------TIER2------------------*/
    /* FIXME _ProfStart(PGROUP_T2); */
    l_data_read = 0;
    if (! opj_tcd_t2_decode(p_tcd, p_src, &l_data_read, p_max_length, p_cstr_index,
                            p_manager)) {
        return OPJ_FALSE;
    }
    /* FIXME _ProfStop(PGROUP_T2); */

    /*------------------TIER1-----------------*/

    /* FIXME _ProfStart(PGROUP_T1); */
    if (! opj_tcd_t1_decode(p_tcd, p_manager)) {
        return OPJ_FALSE;
    }
    /* FIXME _ProfStop(PGROUP_T1); */

    /*----------------DWT---------------------*/

    /* FIXME _ProfStart(PGROUP_DWT); */
    if
    (! opj_tcd_dwt_decode(p_tcd)) {
        return OPJ_FALSE;
    }
    /* FIXME _ProfStop(PGROUP_DWT); */

    /*----------------MCT-------------------*/
    /* FIXME _ProfStart(PGROUP_MCT); */
    if
    (! opj_tcd_mct_decode(p_tcd, p_manager)) {
        return OPJ_FALSE;
    }
    /* FIXME _ProfStop(PGROUP_MCT); */

    /* FIXME _ProfStart(PGROUP_DC_SHIFT); */
    if
    (! opj_tcd_dc_level_shift_decode(p_tcd)) {
        return OPJ_FALSE;
    }
    /* FIXME _ProfStop(PGROUP_DC_SHIFT); */


    /*---------------TILE-------------------*/
    return OPJ_TRUE;
}
