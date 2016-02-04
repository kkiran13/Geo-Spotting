#include <iostream>
#include <string>

using namespace std;

class AreaPopularityCalcHolder{
private:

	int place_id;
	double latitude;
	double longitude;
	int areaPopularityCount;
	int total_check_ins;

public:
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

	int getAreaPopularityCount() const {
		return areaPopularityCount;
	}

	void setAreaPopularityCount(int areaPopularityCount) {
		this->areaPopularityCount = areaPopularityCount;
	}

	int getTotalCheckIns() const {
		return total_check_ins;
	}

	void setTotalCheckIns(int totalCheckIns) {
		total_check_ins = totalCheckIns;
	}
};
