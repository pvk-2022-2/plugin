format_version = "2.0"



custom_properties = jbox.property_set{
	document_owner = {
		properties = {
			
			OnOff = jbox.boolean{
				property_tag = 1,
				default = true,
				ui_name = jbox.ui_text("text_OnOff"),
				ui_type = jbox.ui_selector{
					jbox.ui_text("text_Off"), jbox.ui_text("text_On")
				},
			},

			beat_numerator = jbox.number{
				property_tag=101,
				default = 3,
				steps = 16,
				ui_name = jbox.ui_text("text_BeatNumerator"),
				ui_type = jbox.ui_linear({min=1, max=16, units={{decimals=0}}}),
			},
			beat_denominator = jbox.number{
				property_tag=102,
				default = 3,
				steps = 16,
				ui_name = jbox.ui_text("text_BeatDenominator"),
				ui_type = jbox.ui_linear({min=1, max=16, units={{decimals=0}}}),
			},
			program = jbox.string { -- string that contains a program to be loaded
				default = "",
				property_tag = 1000
			},
		},
	},

	rtc_owner = {
		properties = {
			instance = jbox.native_object{ },
		}
	},	
	
	rt_owner = {
		properties = {
			text_out_buffer = jbox.string{
				max_size = 1024
			},
			grid_data = jbox.string{
				max_size = 1024
			},
		}
	},
}

midi_implementation_chart = {
	midi_cc_chart = {
		[40] = "/custom_properties/beat_numerator",
		[41] = "/custom_properties/beat_denominator",
	}
}

remote_implementation_chart = {
}

ui_groups = {{
		-- We have so few properties we don't need grouping.
		ui_name = jbox.ui_text("text_GroupNameNotUsed"),
		properties = {}
	},
}

cv_inputs = {
	numerator_note_cv = jbox.cv_input{
		ui_name = jbox.ui_text("text_BeatNumerator")
	},
	denominator_note_cv = jbox.cv_input{
		ui_name = jbox.ui_text("text_BeatDenominator")
	},
}



