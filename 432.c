void OverlayWindowViews::OnNativeWidgetMove() {
  if (is_initialized_)
    UpdateControlsVisibility(false);

  window_bounds_ = GetBounds();

#if defined(OS_CHROMEOS)
  WindowQuadrant quadrant = GetCurrentWindowQuadrant(GetBounds(), controller_);
  close_controls_view_->SetPosition(GetBounds().size(), quadrant);
  resize_handle_view_->SetPosition(GetBounds().size(), quadrant);
#endif
}
