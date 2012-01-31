linux_CFLAGS=-pipe -std=$(C_STANDARD)
linux_CXXFLAGS=-pipe -std=$(CXX_STANDARD)

ifneq ($(LTO),)
