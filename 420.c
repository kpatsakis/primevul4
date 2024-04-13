gfx::Rect OverlayWindowViews::GetSecondCustomControlsBounds() {
  if (!second_custom_controls_view_)
    return gfx::Rect();
  return second_custom_controls_view_->GetMirroredBounds();
}
