void OverlayWindowViews::SetPictureInPictureCustomControls(
    const std::vector<blink::PictureInPictureControlInfo>& controls) {
  first_custom_controls_view_.reset();
  second_custom_controls_view_.reset();

  if (controls.size() > 0)
    CreateCustomControl(first_custom_controls_view_, controls[0],
                        ControlPosition::kLeft);
  if (controls.size() > 1)
    CreateCustomControl(second_custom_controls_view_, controls[1],
                        ControlPosition::kRight);
}
