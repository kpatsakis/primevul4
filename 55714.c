static void opj_tcd_code_block_dec_deallocate(opj_tcd_precinct_t * p_precinct)
{
    OPJ_UINT32 cblkno, l_nb_code_blocks;

    opj_tcd_cblk_dec_t * l_code_block = p_precinct->cblks.dec;
    if (l_code_block) {
        /*fprintf(stderr,"deallocate codeblock:{\n");*/
        /*fprintf(stderr,"\t x0=%d, y0=%d, x1=%d, y1=%d\n",l_code_block->x0, l_code_block->y0, l_code_block->x1, l_code_block->y1);*/
        /*fprintf(stderr,"\t numbps=%d, numlenbits=%d, len=%d, numnewpasses=%d, real_num_segs=%d, m_current_max_segs=%d\n ",
                        l_code_block->numbps, l_code_block->numlenbits, l_code_block->len, l_code_block->numnewpasses, l_code_block->real_num_segs, l_code_block->m_current_max_segs );*/


        l_nb_code_blocks = p_precinct->block_size / sizeof(opj_tcd_cblk_dec_t);
        /*fprintf(stderr,"nb_code_blocks =%d\t}\n", l_nb_code_blocks);*/

        for (cblkno = 0; cblkno < l_nb_code_blocks; ++cblkno) {

            if (l_code_block->segs) {
                opj_free(l_code_block->segs);
                l_code_block->segs = 00;
            }

            if (l_code_block->chunks) {
                opj_free(l_code_block->chunks);
                l_code_block->chunks = 00;
            }

            ++l_code_block;
        }

        opj_free(p_precinct->cblks.dec);
        p_precinct->cblks.dec = 00;
    }
}
