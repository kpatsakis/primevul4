av_cold void ff_mpegvideodsp_init_x86(MpegVideoDSPContext *c)
{
#if HAVE_INLINE_ASM
    int cpu_flags = av_get_cpu_flags();

    if (INLINE_MMX(cpu_flags))
        c->gmc = gmc_mmx;
#endif /* HAVE_INLINE_ASM */
}
