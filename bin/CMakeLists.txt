add_executable(itmoloops
    main.cpp
    "../lib/fileparser.cpp"
)

target_link_libraries(itmoloops PRIVATE itmoloops_lib)
