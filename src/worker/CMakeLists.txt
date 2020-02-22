add_library(worker Container.cpp Docker.cpp Checkpoint.cpp)
target_link_libraries(worker Qt5::Core Qt5::Network configfiles commandLineInterface)