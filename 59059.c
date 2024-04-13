static void labels_to_offsets(void)
{
   struct unfold_elm *ue;
   struct unfold_elm *s;
   u_int32 offset = 0;

   fprintf(stdout, " Converting labels to real offsets ");
   fflush(stdout);
   
   TAILQ_FOREACH(ue, &unfolded_tree, next) {
      /* search only for jumps */
      if (ue->fop.opcode == FOP_JMP || 
          ue->fop.opcode == FOP_JTRUE ||
          ue->fop.opcode == FOP_JFALSE) {
        
         switch (ue->fop.opcode) {
            case FOP_JMP:
               ef_debug(1, "*"); 
               break;
            case FOP_JTRUE:
               ef_debug(1, "+");
               break;
            case FOP_JFALSE:
               ef_debug(1, "-");
               break;
         }
         
         /* search the offset associated with the label */
         TAILQ_FOREACH(s, &unfolded_tree, next) {
            if (s->label == ue->fop.op.jmp) {
               ue->fop.op.jmp = offset;
               /* reset the offset */
               offset = 0;
               break;
            }
            /* if it is an instruction, increment the offset */
            if (s->label == 0)
               offset++;
         }
      }
   }

   fprintf(stdout, " done.\n\n");
}
