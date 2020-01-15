#pragma once


enum JobStatus {
	scheduled,
	processing,
	paused,
	interrupted,
	finished,
	canceled,
};