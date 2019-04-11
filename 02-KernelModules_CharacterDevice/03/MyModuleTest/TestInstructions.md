Testing instructions for a charachter device module

1. Compile the module using "$make" command (path to built kernel should be exported in BUILD_KERNEL variable)

2. Export .ko file path in MODULE_PATH variable (this and all further actions should be done from root user - use "$sudo su" beforehand!)

3. Run "device_test.sh" script file

4. Compare the outputs of each step with what script outputs suggest they should be
