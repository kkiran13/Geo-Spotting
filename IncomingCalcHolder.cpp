#include <iostream>

class IncomingCalcHolder{
private:
	int placeId;
	double latitude;
	double longitude;
	int incominddata;

public:
	int getIncominddata() const {
		return incominddata;
	}

	void setIncominddata(int incominddata) {
		this->incominddata = incominddata;
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
		return placeId;
	}

	void setPlaceId(int placeId) {
		this->placeId = placeId;
	}
};
