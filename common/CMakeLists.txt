add_library(common STATIC
	common_types.h)
set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
create_target_directory_groups(common)

