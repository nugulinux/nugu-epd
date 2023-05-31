ADD_EXECUTABLE(epd_test main.cc epd_process.c epd_util.c)
TARGET_COMPILE_OPTIONS(epd_test PRIVATE
    -g -Os
    -fPIC

    # Improved version of “-fstack-protector” since gcc 4.9
    -fstack-protector-strong

    # Store the stack frame pointer in a register.
    -fno-omit-frame-pointer

    # Eliminate unused code and data (with --gc-sections link option)
    -fdata-sections -ffunction-sections

    # Increase reliability of backtraces
    -fasynchronous-unwind-tables

    # Turn on all warnings and treat all warnings as errors.
    -Werror -Wall -Wextra

    # Additional warnings
    -Wunreachable-code -Wpointer-arith
    -Wformat=2 -Wno-format-nonliteral

    # Not warnings
    -Wno-missing-field-initializers
    -Wno-unused-parameter
)

TARGET_LINK_LIBRARIES(epd_test nugu-epd)
ADD_DEPENDENCIES(epd_test nugu-epd)
