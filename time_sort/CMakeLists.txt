add_executable(time_sort
        main.cpp
	time_sort.cpp
	time_sort.h)

target_link_libraries(time_sort PRIVATE common fmt ${ROOT_LIBRARIES})
create_target_directory_groups(time_sort)
