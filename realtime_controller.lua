format_version = "1.0"

rtc_bindings = {
    { source = "/environment/system_sample_rate", dest = "/global_rtc/init_instance" },
    { source = "/environment/instance_id", dest = "/global_rtc/init_instance" },
}


global_rtc = {
    -- Function to create a new instance of the 45 if the sample rate changes
    init_instance = function (source_property_path, new_value)
        local sample_rate = jbox.load_property("/environment/system_sample_rate")
        local instance_id = jbox.load_property("/environment/instance_id")

        local new_no = jbox.make_native_object_rw("Instance", {sample_rate, instance_id})
        jbox.store_property("/custom_properties/instance", new_no);
    end,
}

rt_input_setup = {
	notify = {
        "/note_states/*",
        "/transport/*",
		"/custom_properties/*",
        "/environment/player_bypassed",
	}
}

sample_rate_setup = {
	native = {
		22050,
		44100,
		48000,
		88200,
		96000,
		192000
	},
	
}
