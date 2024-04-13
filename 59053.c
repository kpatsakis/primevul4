struct condition * compiler_concat_conditions(struct condition *a, u_int16 op, struct condition *b)
{
   struct condition *head = a;
   
   /* go to the last conditions in 'a' */
   while(a->next != NULL)
      a = a->next;
   
   /* set the operation */
   a->op = op;

   /* contatenate the two block */
   a->next = b;
   
   /* return the head of the conditions */
   return head;
}
