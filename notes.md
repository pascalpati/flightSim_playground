Build notes:

// Configure the project
cmake --preset vcpkg --log-level=VERBOSE

// Generate the build files (build the project)
cmake -A x64 -B ./build .

// Build the release target
cmake --build ./build --config Release

// Build the debug target
cmake --build ./build --config Debug
