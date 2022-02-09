format_version = "4.0"

panel_type = "note_player"

front = jbox.panel{
	backdrop = jbox.image{path="Reason_GUI_Panel_Front_3U"},
	widgets = {
		jbox.analog_knob{
			transform = {320,60},
			animation = jbox.image_sequence{path = "Reason_GUI_Knob_Green", frames = 63},
			value = "/custom_properties/beat_numerator",
			show_remote_box = true,
			show_automation_rect = true,
		},
		jbox.zero_snap_knob{
			transform = {320,140},
			animation = jbox.image_sequence{path = "Reason_GUI_Knob_Blue", frames = 63},
			value = "/custom_properties/beat_denominator",
			show_remote_box = true,
			show_automation_rect = true,
		},
		jbox.toggle_button{
			transform = {30,4},
			background = jbox.image_sequence{path = "Reason_GUI_ONOFF maker x5 002", frames = 4},
			value = "/custom_properties/OnOff",
			show_remote_box = true,
			show_automation_rect = true,
		},
		jbox.patch_name{
			transform = {127,10},
			center = false,
			fg_color = { 96, 96, 96 },
			height = 16,
			loader_alt_color = { 0, 0, 0 },
			text_style = "Arial medium font",
			width = 114,
		},
		jbox.patch_browse_group{
			transform = {62,7},
			fx_patch = false,
		},
	}
}

back = jbox.panel{
	backdrop = jbox.image{path="Reason_GUI_Panel_Back_3U"},
	widgets = {
		jbox.placeholder{
			transform = {420,40},
		},
		jbox.cv_input_socket{
			transform = {100,20},
			socket = "/cv_inputs/numerator_note_cv",
		},
		jbox.cv_input_socket{
			transform = {100,80},
			socket = "/cv_inputs/denominator_note_cv",
		},
	}
}

