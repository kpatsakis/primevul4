struct ifblock * compiler_create_ifblock(struct condition *conds, struct block *blk)
{
   struct ifblock *ifblk;

   SAFE_CALLOC(ifblk, 1, sizeof(struct ifblock));

   /* associate the pointers */
   ifblk->conds = conds;
   ifblk->blk = blk;

   return ifblk;
}
