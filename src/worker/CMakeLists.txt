add_library(worker Container.cpp Docker.cpp Snapshot.cpp)
target_link_libraries(worker Qt5::Core Qt5::Network configfiles commandLineInterface)