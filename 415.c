gfx::Rect OverlayWindowViews::GetFirstCustomControlsBounds() {
  if (!first_custom_controls_view_)
    return gfx::Rect();
  return first_custom_controls_view_->GetMirroredBounds();
}
