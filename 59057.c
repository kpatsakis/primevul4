struct instruction * compiler_create_instruction(struct filter_op *fop)
{
   struct instruction *ins;

   SAFE_CALLOC(ins, 1, sizeof(struct instruction));
   
   /* copy the instruction */
   memcpy(&ins->fop, fop, sizeof(struct filter_op));

   return ins;
}
