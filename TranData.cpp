#include <iostream>
#include <string>

using namespace std;
class TranData{
	private:

	int placeId1;
	int placeId2;
	string time1;
	string time2;

public:
	int getPlaceId1() const {
		return placeId1;
	}

	void setPlaceId1(int placeId1) {
		this->placeId1 = placeId1;
	}

	int getPlaceId2() const {
		return placeId2;
	}

	void setPlaceId2(int placeId2) {
		this->placeId2 = placeId2;
	}

	const string& getTime1() const {
		return time1;
	}

	void setTime1(const string& time1) {
		this->time1 = time1;
	}

	const string& getTime2() const {
		return time2;
	}

	void setTime2(const string& time2) {
		this->time2 = time2;
	}
};
