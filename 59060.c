static void unfold_blk(struct block **blk)
{
   struct unfold_elm *ue = NULL;
  
   BUG_IF(*blk == NULL);

   /* the progress bar */
   ef_debug(1, "+"); 
   
   do {
      switch((*blk)->type) {
         case BLK_INSTR:
            /* insert the instruction as is */
            SAFE_CALLOC(ue, 1, sizeof(struct unfold_elm));
            memcpy(&ue->fop, (*blk)->un.ins, sizeof(struct filter_op));
            TAILQ_INSERT_TAIL(&unfolded_tree, ue, next);
            break;

         case BLK_IFBLK:
            unfold_ifblk(blk);
            break;
            
         default:
            BUG("undefined tree element");
            break;
      } 
   } while ((*blk = (*blk)->next));
  
}
