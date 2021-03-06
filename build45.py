# Build configuration

# List all C or C++ files here. Wildcards, relative and absolute paths are allowed
SOURCE_FILES = [
	"*.cpp"
	]

# List any extra include directories for clang. Relative and absolute paths are allowed
INCLUDE_DIRS = [
		"./mips-emulator/include/"
	]

# Path to the Jukebox SDK directory. A relative or an absolute path is allowed
JUKEBOX_SDK_DIR = "../.."

# Other compiler flags - please avoid any flag which affects the output for your universal45 builds
OTHER_COMPILER_FLAGS = "-Wall"

# End of build configuration





# Add the build directory to the python path to be able to find the imports below
import os
import sys

# Add option for custom JUKEBOX_SDK_DIR, we must remove related args to please build
args = sys.argv.copy()
for arg in sys.argv:
	if not "=" in arg:
		continue

	split = arg.split("=")
	if not split[0].lower() == "sdk":
		continue

	JUKEBOX_SDK_DIR = split[1]
	args.remove(arg)


importPath = os.path.abspath(os.path.join(JUKEBOX_SDK_DIR, "Tools", "Build"))
sys.path.append(importPath)



# Propagate variables to the main build script
import buildconfig
buildconfig.SOURCE_FILES = SOURCE_FILES
buildconfig.JUKEBOX_SDK_DIR = os.path.normpath(JUKEBOX_SDK_DIR)
buildconfig.INCLUDE_DIRS = INCLUDE_DIRS
buildconfig.OTHER_COMPILER_FLAGS = OTHER_COMPILER_FLAGS

# Clang's static code analysis is turned on by default. Uncomment to temporarily turn off.
# buildconfig.STATIC_ANALYSIS = False

# Default is low verbosity during builds. All errors and warnings are still visible though.
# buildconfig.VERBOSE_BUILD = True

# Start the main build script
import build
build.doBuild(args)
