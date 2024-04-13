struct condition * compiler_create_condition(struct filter_op *fop)
{
   struct condition *cnd;

   SAFE_CALLOC(cnd, 1, sizeof(struct condition));
   
   /* copy the instruction */
   memcpy(&cnd->fop, fop, sizeof(struct filter_op));

   return cnd;
}
