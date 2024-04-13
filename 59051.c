struct block * compiler_add_ifblk(struct ifblock *ifb, struct block *blk)
{
   struct block *bl;

   SAFE_CALLOC(bl, 1, sizeof(struct block));

   /* copy the current instruction in the block */
   bl->type = BLK_IFBLK;
   bl->un.ifb = ifb;

   /* link it to the old block chain */
   bl->next = blk;

   return bl;
}
