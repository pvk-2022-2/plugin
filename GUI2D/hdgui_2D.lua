format_version="2.0"

front = jbox.panel{
	graphics = {
		node = "Surface_Bg",
	},
	
	widgets = {
        jbox.custom_display {
			graphics = { node = "Terminal" },
			values = { "/custom_properties/text_out_buffer" },
			display_width_pixels = 360,
			display_height_pixels = 180,
			draw_function = "draw_terminal",
		},
        jbox.custom_display {
			graphics = { node = "Grid" },
            -- grid_data is data from realtime, while grid_gesture_data is the other way around.
			values = { "/custom_properties/grid_data","/custom_properties/grid_gesture_data" },
			display_width_pixels = 180,
			display_height_pixels = 180,

			draw_function = "draw_grid",
		},
        jbox.toggle_button{
            graphics = {
                node = "OnOff",
            },
            value = "/custom_properties/OnOff",
        },
	},		
}

back = jbox.panel{
	graphics = {
		node = "Surface_Bg"
	},
	widgets = {
        jbox.placeholder{
            graphics = {
                node = "Placeholder",
            },
        },
        jbox.cv_input_socket{
            graphics = {
                node = "CVJack1",
            },
            socket = "/cv_inputs/numerator_note_cv",
        },
        jbox.cv_input_socket{
            graphics = {
                node = "CVJack2",
            },
            socket = "/cv_inputs/denominator_note_cv",
        },
    },
}

