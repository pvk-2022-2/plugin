format_version="2.0"

front = jbox.panel{


	graphics = {
		node = "Surface_Bg",
	},
	
	widgets = {
	
		-------------------------------------------
		-------------------------------------------
		--- RootKeyOffset
		-------------------------------------------

		jbox.analog_knob{
			graphics = {
				node = "Surface_BeatNumerator"
			},
			value = "/custom_properties/beat_numerator",
		},
		jbox.zero_snap_knob{
			graphics = {
				node = "Surface_BeatDenominator"
			},
			value = "/custom_properties/beat_denominator",
		},

        jbox.toggle_button{
            graphics = {
                node = "OnOff",
            },
            value = "/custom_properties/OnOff",
        },

        jbox.patch_name{
            graphics = {
                node = "PatchNameSurface",
            },
            center = false,
            fg_color = { 96, 96, 96 },
            loader_alt_color = { 0, 0, 0 },
            -- text_style = "Big bold LCD font",
            text_style = "Arial medium font",
        },
        jbox.patch_browse_group{
            graphics = {
                node = "PatchBrowseGroup",
            },
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

