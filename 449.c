void OverlayWindowViews::UpdateVideoSize(const gfx::Size& natural_size) {
  DCHECK(!natural_size.IsEmpty());
  natural_size_ = natural_size;
  SetAspectRatio(gfx::SizeF(natural_size_));

  SetBounds(CalculateAndUpdateWindowBounds());
}
