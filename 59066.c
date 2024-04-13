static u_char * create_data_segment(struct filter_header *fh, struct filter_op *fop, size_t n)
{
   size_t i, len = 0;
   u_char *data = NULL;

   for (i = 0; i < n; i++) {
      
      switch(fop[i].opcode) {
         case FOP_FUNC:
            if (fop[i].op.func.slen) {
               ef_debug(1, "@");
               len += add_data_segment(&data, len, &fop[i].op.func.string, fop[i].op.func.slen);
            }
            if (fop[i].op.func.rlen) {
               ef_debug(1, "@");
               len += add_data_segment(&data, len, &fop[i].op.func.replace, fop[i].op.func.rlen);
            }
            break;
            
         case FOP_TEST:
            if (fop[i].op.test.slen) {
               ef_debug(1, "@");
               len += add_data_segment(&data, len, &fop[i].op.test.string, fop[i].op.test.slen);
            }
            break;

         case FOP_ASSIGN:
            if (fop[i].op.assign.slen) {
               ef_debug(1, "@");
               len += add_data_segment(&data, len, &fop[i].op.test.string, fop[i].op.test.slen);
            }
            break;
      }

   }
  
   /* where starts the code ? */
   fh->code = fh->data + len;
   
   return data;
}
