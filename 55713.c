static OPJ_BOOL opj_tcd_code_block_dec_allocate(opj_tcd_cblk_dec_t *
        p_code_block)
{
    if (! p_code_block->segs) {

        p_code_block->segs = (opj_tcd_seg_t *) opj_calloc(OPJ_J2K_DEFAULT_NB_SEGS,
                             sizeof(opj_tcd_seg_t));
        if (! p_code_block->segs) {
            return OPJ_FALSE;
        }
        /*fprintf(stderr, "Allocate %d elements of code_block->data\n", OPJ_J2K_DEFAULT_NB_SEGS * sizeof(opj_tcd_seg_t));*/

        p_code_block->m_current_max_segs = OPJ_J2K_DEFAULT_NB_SEGS;
        /*fprintf(stderr, "m_current_max_segs of code_block->data = %d\n", p_code_block->m_current_max_segs);*/
    } else {
        /* sanitize */
        opj_tcd_seg_t * l_segs = p_code_block->segs;
        OPJ_UINT32 l_current_max_segs = p_code_block->m_current_max_segs;
        opj_tcd_seg_data_chunk_t* l_chunks = p_code_block->chunks;
        OPJ_UINT32 l_numchunksalloc = p_code_block->numchunksalloc;
        OPJ_UINT32 i;

        memset(p_code_block, 0, sizeof(opj_tcd_cblk_dec_t));
        p_code_block->segs = l_segs;
        p_code_block->m_current_max_segs = l_current_max_segs;
        for (i = 0; i < l_current_max_segs; ++i) {
            opj_tcd_reinit_segment(&l_segs[i]);
        }
        p_code_block->chunks = l_chunks;
        p_code_block->numchunksalloc = l_numchunksalloc;
    }

    return OPJ_TRUE;
}
