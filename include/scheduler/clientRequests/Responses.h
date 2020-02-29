#pragma once


// This file contains all responses or partial responses for client requests, for testing mainly but may also improve readability of clientrequest subclasses

constexpr const char JOB_ABORTED[] = "This Job has been aborted.";
constexpr const char JOB_FINISHED[] = "This Job has finished.";
constexpr const char JOB_IN_QUEUE[] = "This Job is currently waiting in the queue.";
constexpr const char JOB_INTERRUPTED[] = "This Job has been interrupted in favor of an emergency schedule.";
constexpr const char JOB_PAUSED[] = "This Job is currently paused.";
constexpr const char JOB_PROCESSING[] = "This Job is currently running.";
constexpr const char JOB_STATUS_UNKNOWN[] = "This Job has an unknown Status. This may be a sign of corruption.";

constexpr const char NO_JOB_ID[] = "Command is missing Job ID. Cannot continue.";
constexpr const char NO_JOB_WITH_ID[] = "Could not find a Job with this jobID.";

constexpr const char OPERATION_ALREADY_APPLIED[] = "This operation has already been applied to this Job.";
constexpr const char OPERATION_FAILURE[] = "Failed to complete the operation.";
constexpr const char OPERATION_SUCCESS[] = "Sucessfully completed the operation.";

constexpr const char OPERATION_UNAVAILABLE_JOB_NOT_PAUSED[] = "Could not perform this operation as the Job has not been paused.";
constexpr const char OPERATION_UNAVAILABLE_JOB_NOT_RUN[] = "Could not perform this operation as the Job has not run yet.";

constexpr const char OPERATION_UNAVAILABLE_JOB_ABORTED[] = "Could not perform this operation as the Job has been aborted.";
constexpr const char OPERATION_UNAVAILABLE_JOB_FINISHED[] = "Could not perform this operation as the Job has been finished.";

constexpr const char OPERATION_PROGRESSING_BACKUP[] = "Backing Job up. Please wait.";
constexpr const char OPERATION_PROGRESSING_PAUSE[] = "Pausing Job. Please wait.";
constexpr const char OPERATION_PROGRESSING_RESTORE[] = "Restoring Job. Please wait.";
constexpr const char OPERATION_PROGRESSING_RESUME[] = "Resuming Job. Please wait.";
constexpr const char OPERATION_PROGRESSING_STOP[] = "Stopping Job. Please wait.";
constexpr const char OPERATION_PROGRESSING_TAIL[] = "Fetching Job Tail. Please wait.";

constexpr const char PREFIX_JOB_EXIT_CODE[] = "The Job has finished with exit code: ";
constexpr const char PREFIX_JOB_ID[] = "The Job ID is: ";
constexpr const char PREFIX_JOB_QUEUE_POS[] = "Position of Job in Queue is: ";
constexpr const char PREFIX_JOB_SUBMISSION_TIME[] = "The Job was submitted at: ";
constexpr const char PREFIX_JOB_START_TIME[] = "The Job was started at: ";