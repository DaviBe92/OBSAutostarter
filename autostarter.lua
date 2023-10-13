obs           = obslua

localSettings = settings
----------------------------------------------------------

-- A function named script_properties defines the properties that the user
-- can change for the entire script module itself
function script_properties()

	local props = obs.obs_properties_create()
	
	obs.obs_properties_add_bool(props, "boolIsEnabled", "Enabled")
	
	obs.obs_properties_add_editable_list(props, "execPaths", "Executables", obs.OBS_EDITABLE_LIST_TYPE_FILES, "*.*", nil)

	obs.obs_properties_add_bool(props, "boolAutoquit", "Autoclose on OBS quit? \n(Only works with .exe files)")

	obs.obs_properties_add_button(props, "launchButton", "Launch", launch_func)
	return props
end

-- A function named script_description returns the description shown to
-- the user
function script_description()
	return "Starts programms when OBS is started.\n\nV0.5  \n\nMade by Davi Be"
end


-- A function named script_defaults will be called to set the default settings
function script_defaults(settings)
end

-- A function named script_update will be called when settings are changed
function script_update(settings)
end


-- a function named script_load will be called on startup
function script_load(settings)
	localSettings = settings
	launch_func()
end

-- save additional data not set by user
function script_save(settings)
end

-- function will be called just before OBS quits
-- quit programm when exiting OBS
function script_unload()

	-- assemble command		
	if obs.obs_data_get_bool(localSettings, "boolAutoquit") then 
		local execPaths = obs.obs_data_get_array(localSettings, 'execPaths')
		local count = obs.obs_data_array_count(execPaths)

		for i = 0, count do 	
			local item = obs.obs_data_array_item(execPaths, i)
			local execPath = obs.obs_data_get_string(item, "value")

			-- replace / by \
			execPath = execPath:gsub("/","\\")
	
			-- only proceed if there is a  file selected
			if execPath == '' then return nil end
		
			-- get Executable
			local index = execPath:match'^.*()\\'
			local execName = execPath:sub(index + 1, execPath:len())
		
			-- assemble command
			local cmd = 'taskkill /IM "' .. execName .. '"'
			
			-- execute command
			os.execute(cmd)
		end 
	end
end

-- function will be called on StartUp
function launch_func()
	local execPaths = obs.obs_data_get_array(localSettings, 'execPaths')
	local count = obs.obs_data_array_count(execPaths)

	if obs.obs_data_get_bool(localSettings, "boolIsEnabled") then
		for i = 0, count do 	
			local item = obs.obs_data_array_item(execPaths, i)
			local execPath = obs.obs_data_get_string(item, "value")
		
			-- replace / by \
			execPath = execPath:gsub("/","\\")
	
			-- obs.script_log(obs.LOG_INFO, execPath)
		
			-- only proceed if there is a  file selected
			if execPath == '' then return nil end
		
			-- get Directory
			local index = execPath:match'^.*()\\'
			local execDir = execPath:sub(1,index)
		
			-- get Executable
			local execName = execPath:sub(index + 1, execPath:len())
		
			-- assemble command
			local cmd = 'start "" /D "' .. execDir .. '" "' .. execName .. '"'
			
			-- execute command
			os.execute(cmd)
		end
	end

	obs.obs_data_array_release(execPaths)
end