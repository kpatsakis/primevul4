static void CalculateColors(unsigned short c0, unsigned short c1,
  DDSColors *c, MagickBooleanType ignoreAlpha)
{
  c->a[0] = c->a[1] = c->a[2] = c->a[3] = 0;

  c->r[0] = (unsigned char) C565_red(c0);
  c->g[0] = (unsigned char) C565_green(c0);
  c->b[0] = (unsigned char) C565_blue(c0);

  c->r[1] = (unsigned char) C565_red(c1);
  c->g[1] = (unsigned char) C565_green(c1);
  c->b[1] = (unsigned char) C565_blue(c1);

  if (ignoreAlpha != MagickFalse || c0 > c1)
    {
      c->r[2] = (unsigned char) ((2 * c->r[0] + c->r[1]) / 3);
      c->g[2] = (unsigned char) ((2 * c->g[0] + c->g[1]) / 3);
      c->b[2] = (unsigned char) ((2 * c->b[0] + c->b[1]) / 3);

      c->r[3] = (unsigned char) ((c->r[0] + 2 * c->r[1]) / 3);
      c->g[3] = (unsigned char) ((c->g[0] + 2 * c->g[1]) / 3);
      c->b[3] = (unsigned char) ((c->b[0] + 2 * c->b[1]) / 3);
    }
  else
    {
      c->r[2] = (unsigned char) ((c->r[0] + c->r[1]) / 2);
      c->g[2] = (unsigned char) ((c->g[0] + c->g[1]) / 2);
      c->b[2] = (unsigned char) ((c->b[0] + c->b[1]) / 2);

      c->r[3] = c->g[3] = c->b[3] = 0;
      c->a[3] = 255;
    }
}
