#pragma once


class Container {
public:
	string Tail(int lines);

	void Stop();

	Snapshot Snapshot(bool stop = false);

	Resume(const Snapshot& snap);


private:
	string id;

	<Worker*>;

};