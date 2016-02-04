#include <iostream>
#include <string>

using namespace std;

class DensityCalcHolder{
private:

	int place_id;
	double latitude;
	double longitude;
	int density;

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

	int getDensity() const {
		return density;
	}

	void setDensity(int density) {
		this->density = density;
	}
};
