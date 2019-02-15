add_executable(tof
        main.cpp
	    tof.cpp
	    tof.h)

target_link_libraries(tof PRIVATE common fmt ${ROOT_LIBRARIES})
create_target_directory_groups(tof)

