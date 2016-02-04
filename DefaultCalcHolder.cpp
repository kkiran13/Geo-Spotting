#include <iostream>
#include <string>

using namespace std;

class DefaultCalcHolder{
private:

	int place_id;
	double latitude;
	double longitude;
	int totalCheckins;
	int checkIns;
	string category;

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

	int getTotalCheckins() const {
		return totalCheckins;
	}

	void setTotalCheckins(int totalCheckins) {
		this->totalCheckins = totalCheckins;
	}

	int getCheckIns() const {
		return checkIns;
	}

	void setCheckIns(int checkIns) {
		this->checkIns = checkIns;
	}
};
