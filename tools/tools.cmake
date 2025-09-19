cmake_minimum_required(VERSION 3.25)

# Create a user-editable variable to allow for the path to the Python virtual
# environment to be customized by passing -DVENV_PATH=... to CMake.
set(
	VENV_PATH "${PROJECT_SOURCE_DIR}/env"
	CACHE PATH "Directory containing Python virtual environment"
)

# If no virtual environment was activated prior to building, attempt to set up
# the one specified by the variable.
if(NOT IS_DIRECTORY "$ENV{VIRTUAL_ENV}")
	if(IS_DIRECTORY "${VENV_PATH}")
		set(ENV{VIRTUAL_ENV} "${VENV_PATH}")
	else()
		message(FATAL_ERROR "Unable to find the Python virtual environment. \
Refer to the README to set one up in ${VENV_PATH}, or pass -DVENV_PATH=... to \
CMake to specify its location manually.")
	endif()
endif()

# Activate the environment by letting CMake search for its Python interpreter.
set(Python3_FIND_VIRTUALENV ONLY)
find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)

# Define some helper functions that rely on the Python scripts in the tools
# folder.

function(convertasset input output)
    add_custom_command(
        OUTPUT "${output}"
        DEPENDS "${PROJECT_SOURCE_DIR}/${input}"
        COMMAND "${Python3_EXECUTABLE}"
                "${PROJECT_SOURCE_DIR}/tools/convasset.py"
                "${PROJECT_SOURCE_DIR}/${input}"
                "${PROJECT_BINARY_DIR}/${output}"
        VERBATIM
    )
endfunction()