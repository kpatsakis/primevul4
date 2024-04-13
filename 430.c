void OverlayWindowViews::OnNativeBlur() {
  if (is_initialized_)
    UpdateControlsVisibility(false);

  views::Widget::OnNativeBlur();
}
