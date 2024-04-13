bool OverlayWindowViews::HasOnlyOneCustomControl() {
  return first_custom_controls_view_ && !second_custom_controls_view_;
}
