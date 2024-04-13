void OverlayWindowViews::UpdateControlsBounds() {
  gfx::Rect larger_window_bounds = GetBounds();
  larger_window_bounds.Inset(-1, -1);
  controls_scrim_view_->SetBoundsRect(
      gfx::Rect(gfx::Point(0, 0), larger_window_bounds.size()));

  WindowQuadrant quadrant = GetCurrentWindowQuadrant(GetBounds(), controller_);
  close_controls_view_->SetPosition(GetBounds().size(), quadrant);
#if defined(OS_CHROMEOS)
  resize_handle_view_->SetPosition(GetBounds().size(), quadrant);
#endif

  controls_parent_view_->SetBoundsRect(
      gfx::Rect(gfx::Point(0, 0), GetBounds().size()));

  UpdateControlsPositions();
}
