int CLASS flip_index (int row, int col)
{
  if (flip & 4) SWAP(row,col);
  if (flip & 2) row = iheight - 1 - row;
  if (flip & 1) col = iwidth  - 1 - col;
  return row * iwidth + col;
}
