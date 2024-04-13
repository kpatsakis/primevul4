void OverlayWindowViews::UpdateControlsPositions() {
  int mid_window_x = GetBounds().size().width() / 2;

  if (HasOnlyOneCustomControl()) {
    play_pause_controls_view_->SetBoundsRect(
        CalculateControlsBounds(mid_window_x, button_size_));
    first_custom_controls_view_->SetBoundsRect(CalculateControlsBounds(
        mid_window_x - button_size_.width(), button_size_));
    return;
  }

  play_pause_controls_view_->SetBoundsRect(CalculateControlsBounds(
      mid_window_x - button_size_.width() / 2, button_size_));

  if (first_custom_controls_view_ && second_custom_controls_view_) {
    first_custom_controls_view_->SetBoundsRect(CalculateControlsBounds(
        mid_window_x - button_size_.width() / 2 - button_size_.width(),
        button_size_));
    second_custom_controls_view_->SetBoundsRect(CalculateControlsBounds(
        mid_window_x + button_size_.width() / 2, button_size_));
  }
}
