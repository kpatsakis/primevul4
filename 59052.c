struct block * compiler_add_instr(struct instruction *ins, struct block *blk)
{
   struct block *bl;

   SAFE_CALLOC(bl, 1, sizeof(struct block));

   /* copy the current instruction in the block */
   bl->type = BLK_INSTR;
   bl->un.ins = ins;

   /* link it to the old block chain */
   bl->next = blk;

   return bl;
}
