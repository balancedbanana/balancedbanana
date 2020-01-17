#pragma once


class Snapshot {
private:
	string id;


public:
	inline const string get_id() const;

	Snapshot(const std::string& id);

};
inline const string Snapshot::get_id() const {
	return id;
}