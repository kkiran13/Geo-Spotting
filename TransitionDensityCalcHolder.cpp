#include <iostream>

using namespace std;

class TransitionDensityCalcHolder{
private:

	int place_id;
	double latitude;
	double longitude;
	double transitionScore;

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

	double getTransitionScore() const {
		return transitionScore;
	}

	void setTransitionScore(double transitionScore) {
		this->transitionScore = transitionScore;
	}
};
