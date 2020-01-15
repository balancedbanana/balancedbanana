#pragma once

#include <cinttypes>
#include <string>
#include <chrono>
#include <Specs.h>
#include <ctime>

//This is a struct that includes all relevant information about a Job.
struct job_details {
	JobConfig config;

	uint64_t user_id;

	//Represented with an ID.
	int status;

	//The id of the Job.
	uint64_t id;

	std::string command;

	std::chrono::time_point<std::chrono::system_clock> schedule_time;

	std::chrono::time_point<std::chrono::system_clock> start_time;

	std::chrono::time_point<std::chrono::system_clock> finish_time;

	specs allocated_specs;

};