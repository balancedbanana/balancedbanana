#pragma once


//This is a struct that includes all relevant information about a Job.
struct job_details {
	JobConfig config;

	uint64_t user_id;

	//Represented with an ID.
	int status;

	//The id of the Job.
	uint64_t id;

	std::string command;

	std::chrono::time_point schedule_time;

	std::chrono::time_point start_time;

	std::chrono::time_point finish_time;

	specs allocated_specs;

	specs*;

};