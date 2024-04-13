gfx::Rect OverlayWindowViews::CalculateControlsBounds(int x,
                                                      const gfx::Size& size) {
  return gfx::Rect(
      gfx::Point(x, (GetBounds().size().height() - size.height()) / 2), size);
}
