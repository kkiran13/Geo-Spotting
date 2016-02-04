#include <iostream>
#include <string>

using namespace std;

class CompetetivenessCalcHolder{
private:

	int place_id;
	double latitude;
	double longitude;
	double competetiveScore;
	string category;

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

	double getCompetetiveScore() const {
		return competetiveScore;
	}

	void setCompetetiveScore(double competetiveScore) {
		this->competetiveScore = competetiveScore;
	}

	const string& getCategory() const {
		return category;
	}

	void setCategory(const string& category) {
		this->category = category;
	}
};
