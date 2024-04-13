free_bodies(VarLenData **bodies, int len)
{
    while (len--)
    {
        XFREE(bodies[len]->data);
        XFREE(bodies[len]);
    }
}
