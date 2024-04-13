void OverlayWindowViews::UpdateButtonSize() {
  const gfx::Size window_size = GetBounds().size();
  int scaled_button_dimension =
      window_size.width() < window_size.height()
          ? window_size.width() * kControlRatioToWindow
          : window_size.height() * kControlRatioToWindow;

  int new_button_dimension =
      std::max(kMinControlButtonSize, scaled_button_dimension);

  button_size_.SetSize(new_button_dimension, new_button_dimension);
}
