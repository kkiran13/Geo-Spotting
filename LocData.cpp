#include <iostream>
#include <string>

using namespace std;

class LocData{
private:

	int place_id;
	double latitude;
	double longitude;
	string category;
	int unique_users;
	int total_check_ins;
	string title;

public:
	const string& getCategory() const {
		return category;
	}

	void setCategory(const string& category) {
		this->category = category;
	}

	double getLatitude() const {
		return latitude;
	}

	void setLatitude(double latitude) {
		this->latitude = latitude;
	}

	double getLongitude() const {
		return longitude;
	}

	void setLongitude(double longitude) {
		this->longitude = longitude;
	}

	int getPlaceId() const {
		return place_id;
	}

	void setPlaceId(int placeId) {
		place_id = placeId;
	}

	int getTotalCheckIns() const {
		return total_check_ins;
	}

	void setTotalCheckIns(int totalCheckIns) {
		total_check_ins = totalCheckIns;
	}

	int getUniqueUsers() const {
		return unique_users;
	}

	void setUniqueUsers(int uniqueUsers) {
		unique_users = uniqueUsers;
	}

	const string& getTitle() const {
		return title;
	}

	void setTitle(const string& title) {
		this->title = title;
	}
};
