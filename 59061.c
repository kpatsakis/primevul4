static void unfold_ifblk(struct block **blk)
{
   struct ifblock *ifblk;
   struct unfold_elm *ue;
   u_int32 a = vlabel++; 
   u_int32 b = vlabel++; 
   u_int32 c = vlabel++; 

   /*
    * the virtual labels represent the three points of an if block:
    *
    *    if (conds) {
    * a ->
    *       ...
    *       jmp c;
    * b ->
    *    } else {
    *       ...
    *    }
    * c ->
    *
    * if the conds are true, jump to 'a'
    * if the conds are false, jump to 'b'
    * 'c' is used to skip the else if the conds were true
    */

   /* the progress bar */
   ef_debug(1, "#"); 
   
   /* cast the if block */
   ifblk = (*blk)->un.ifb;
  
   /* compile the conditions */
   unfold_conds(ifblk->conds, a, b);
   
   /* if the conditions are match, jump here */
   SAFE_CALLOC(ue, 1, sizeof(struct unfold_elm));
   ue->label = a;
   TAILQ_INSERT_TAIL(&unfolded_tree, ue, next);

   /* check if the block is empty. i.e.  { } */
   if (ifblk->blk != NULL) {
      /* recursively compile the main block */
      unfold_blk(&ifblk->blk);
   }

   /* 
    * if there is the else block, we have to skip it
    * if the condition was true
    */
   if (ifblk->elseblk != NULL) {
      SAFE_CALLOC(ue, 1, sizeof(struct unfold_elm));
      ue->fop.opcode = FOP_JMP;
      ue->fop.op.jmp = c;
      TAILQ_INSERT_TAIL(&unfolded_tree, ue, next);
   }
   
   /* if the conditions are NOT match, jump here (after the block) */
   SAFE_CALLOC(ue, 1, sizeof(struct unfold_elm));
   ue->label = b;
   TAILQ_INSERT_TAIL(&unfolded_tree, ue, next);
   
   /* recursively compile the else block */
   if (ifblk->elseblk != NULL) {
      unfold_blk(&ifblk->elseblk);
      /* this is the label to skip the else if the condition was true */
      SAFE_CALLOC(ue, 1, sizeof(struct unfold_elm));
      ue->label = c;
      TAILQ_INSERT_TAIL(&unfolded_tree, ue, next);
   }

}
