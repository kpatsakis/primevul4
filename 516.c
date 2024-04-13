OverlayWindowViews::OverlayWindowViews(
    content::PictureInPictureWindowController* controller)
    : controller_(controller),
      window_background_view_(new views::View()),
      video_view_(new views::View()),
      controls_scrim_view_(new views::View()),
      controls_parent_view_(new views::View()),
      close_controls_view_(new views::CloseImageButton(this)),
#if defined(OS_CHROMEOS)
      resize_handle_view_(new views::ResizeHandleButton(this)),
#endif
      play_pause_controls_view_(new views::ToggleImageButton(this)),
      hide_controls_timer_(
          FROM_HERE,
          base::TimeDelta::FromMilliseconds(2500 /* 2.5 seconds */),
          base::BindRepeating(&OverlayWindowViews::UpdateControlsVisibility,
                              base::Unretained(this),
                              false /* is_visible */)) {
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_WINDOW);
  params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
  params.bounds = CalculateAndUpdateWindowBounds();
  params.keep_on_top = true;
  params.visible_on_all_workspaces = true;
  params.remove_standard_frame = true;
  params.name = "PictureInPictureWindow";

  params.delegate = new OverlayWindowWidgetDelegate(this);

  Init(params);
  SetUpViews();

#if defined(OS_CHROMEOS)
  GetNativeWindow()->SetProperty(ash::kWindowPipTypeKey, true);
#endif  // defined(OS_CHROMEOS)

  is_initialized_ = true;
}
