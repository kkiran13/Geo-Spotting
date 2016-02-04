#include <iostream>
#include <string>
#include <map>

using namespace std;

class TransitionQualityCalcHolder{
private:

	int place_id;
	double latitude;
	double longitude;
	double transitionQualityScore;

public:
	map<string, int> neighborsAndCount;

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

	double getTransitionQualityScore() const {
		return transitionQualityScore;
	}

	void setTransitionQualityScore(double transitionQualityScore) {
		this->transitionQualityScore = transitionQualityScore;
	}
};
