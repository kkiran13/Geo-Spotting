//============================================================================
// Name        : GeoSpotting.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <omp.h>
#include <sys/time.h>

#include "LocData.cpp"
#include "TranData.cpp"
#include "DensityCalcHolder.cpp"
#include "CompetetivenessCalcHolder.cpp"
#include "NeighborsEntCalcHolder.cpp"
#include "AreaPopularityCalcHolder.cpp"
#include "TransitionDensityCalcHolder.cpp"
#include "IncomingCalcHolder.cpp"
#include "TransitionQualityCalcHolder.cpp"
#include "DefaultCalcHolder.cpp"

using namespace std;

#define EARTH_RADIUS 6371;

map<int, LocData> locDataMap;
map<int, map<int,int> > tranMapSourceToDest;
map<int, map<int,int> > tranMapDestToSource;
string allAlpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

double degToRads(double degs);
double calculateDistance(double lat1, double lon1, double lat2, double lon2);
void calcDefaultRank();

// geo
void calculateDensity();
void calculateCompetetiveness();
void calculateNeighborsEntropy();

// mobile
void calculateAreaPopularity();
void calculateTransitionDensity();
void calculateIncomingFlow();
void calculateTransitionQuality(string category);

int main(int argc, char** argv) {

	if(argc != 4){
		cout << "The correct way to invoke this program is ./Geospotting new_york_locdata_file_name new_york_placenet_file_name category\n";
		exit(0);
	}
	
	int num_of_threads = omp_get_num_threads();
	cout << "Number of OpenMP threads: " << num_of_threads << "\n";
	struct timeval time_start;
	double t_prime_start;


		gettimeofday(&time_start, NULL);
		t_prime_start = (time_start.tv_sec * 1000000.0) + time_start.tv_usec;



	string inp_file = argv[1];
	ifstream locFile(inp_file.c_str());
	string line;
	if (locFile.is_open()) {
		int i = 1;
		while (getline(locFile, line)){// && i <= 100){
			LocData locData;
			char* token = strtok(const_cast<char*>(line.c_str()), "('*;, ");
			int cnt = 1;
			while (token != NULL) {

				// to handle multiple words in category
				if (cnt == 5) {
					string temp = token;
					if (string::npos == temp.find_first_not_of(allAlpha)) {
						cnt--;
					}
				}

				switch (cnt) {
				case 1:
					locData.setPlaceId(atoi(token));
					break;
				case 2:
					locData.setLatitude(atof(token));
					break;
				case 3:
					locData.setLongitude(atof(token));
					break;
				case 4:
					locData.setCategory(locData.getCategory() + "_" + token);
					break;
				case 5:
					locData.setUniqueUsers(atoi(token));
					break;
				case 6:
					locData.setTotalCheckIns(atoi(token));
					break;
				default:
					locData.setTitle(locData.getTitle() + "_" + token);
					break;
				}
				token = strtok(NULL, "('*;,) ");
				cnt++;
			}
			locDataMap.insert(
					pair<int, LocData>(locData.getPlaceId(), locData));
			//cout << locData.getCategory() << endl;
			i++;
		}
		cout << "Loc map details created with " << locDataMap.size() << " entries\n";
	}
		inp_file = argv[2];
		ifstream tranFile(inp_file.c_str());
		int totTranEntry = 0;

		if (tranFile.is_open()) {
			while (getline(tranFile, line)) {
				totTranEntry++;
				char* token = strtok(const_cast<char*>(line.c_str()), ",");

				int placeId1 = atoi(token);
				token = strtok(NULL, ",");
				int placeId2 = atoi(token);

				// fill source to dest map
				if (tranMapSourceToDest.count(placeId1) == 0) {
					map<int, int> tempMap;
					tempMap.insert(pair<int, int>(placeId2, 1));
					tranMapSourceToDest.insert(pair<int, map<int, int> >(placeId1, tempMap));
				} else {
					if (tranMapSourceToDest[placeId1].count(placeId2) == 0) {
						tranMapSourceToDest[placeId1].insert(pair<int, int>(placeId2, 1));
					} else {
						int count = tranMapSourceToDest[placeId1][placeId2];
						tranMapSourceToDest[placeId1][placeId2] = count + 1;
					}
				}

				// fill dest to source map
				if(tranMapDestToSource.count(placeId2) == 0){
					map<int, int> tempMap;
					tempMap.insert(pair<int, int>(placeId1, 1));
					tranMapDestToSource.insert(pair<int, map<int, int> >(placeId2, tempMap));
				}else{
					if(tranMapDestToSource[placeId2].count(placeId1) == 0){
						tranMapDestToSource[placeId2].insert(pair<int, int>(placeId1, 1));
					}else{
						int count = tranMapDestToSource[placeId2][placeId1];
						tranMapDestToSource[placeId2][placeId1] = count +1;
					}
				}
			}
		}

		string category_under_consideration = argv[3];
		cout << "Category passed = " + category_under_consideration << "\n";

		calcDefaultRank();
		calculateDensity();
		calculateCompetetiveness();
		calculateNeighborsEntropy();
		calculateAreaPopularity();
		calculateTransitionDensity();
		calculateIncomingFlow();
		calculateTransitionQuality(category_under_consideration);



	struct timeval time_end;
	double t_prime_end;


		gettimeofday(&time_end, NULL);
		t_prime_end = (time_end.tv_sec * 1000000.0) + time_end.tv_usec;
		double total_time = t_prime_end - t_prime_start;
		cout << "\nTotal time: " << static_cast<long>(total_time) << " usec\n";

	return 0;
}

void calculateTransitionQuality(string categoryToCheck){

	double neighborhood_distance = 200.0;
	cout << "Calculate Transition Quality for category: " << categoryToCheck << " called \n";

	map<string, vector<double> > probMap;
	// key = category of the place from where transition happens to the category under consideration
	// vector[0] = total transitions from source (key), vector[1] = transitions to the cat under consideration

	for(map<int, map<int, int> >::iterator it = tranMapSourceToDest.begin(); it != tranMapSourceToDest.end(); ++it){
		string srcCategory = locDataMap[it->first].getCategory();
		// see if that cat entry exists
		if(probMap.count(srcCategory) == 0){
			vector<double> temp;
			temp.push_back(0.0);
			temp.push_back(0.0);
			probMap.insert(pair<string, vector<double> >(srcCategory, temp));
		}

		// now loop through the inner map to fill the prob vector
		for(map<int, int>::iterator it_inner = it->second.begin(); it_inner != it->second.end(); ++it_inner){
			string destCategory = locDataMap[it_inner->first].getCategory();
			// goes to the required category
			if(destCategory.compare(categoryToCheck) == 0){
				probMap[srcCategory][0] += (double)it_inner->second;
				probMap[srcCategory][1] += (double)it_inner->second;
			}else{
				probMap[srcCategory][0] += (double)it_inner->second;
			}
		}
	}

	// now calculate the probability and set it in vector[0]
	for(map<string, vector<double> >::iterator it = probMap.begin(); it != probMap.end(); ++it){
		if(it->second[1] >= 1.0){
			it->second[0] = it->second[1] / it->second[0];
		}else{
			it->second[0] = 0.0;
		}
	}

	// now that the probabilities are calculated calculate the transition quality
	// make a vector that can be parallelized
	vector<TransitionQualityCalcHolder> TQvector;
	for(map<int, LocData>::iterator it = locDataMap.begin(); it != locDataMap.end(); ++it){
		TransitionQualityCalcHolder TQHolder;
		TQHolder.setPlaceId(it->second.getPlaceId());
		TQHolder.setLatitude(it->second.getLatitude());
		TQHolder.setLongitude(it->second.getLongitude());
		TQHolder.setTransitionQualityScore(0.0);
		TQvector.push_back(TQHolder);
	}

#pragma omp parallel for
	for (int outter = 0; outter < TQvector.size(); outter++) {

		for(map<int, LocData>::iterator it = locDataMap.begin(); it != locDataMap.end(); ++it){
				if (TQvector[outter].getPlaceId()
						== it->second.getPlaceId()) {

					continue;
				}
				if (calculateDistance(TQvector[outter].getLatitude(),
						TQvector[outter].getLongitude(),
						it->second.getLatitude(),
						it->second.getLongitude())
						< neighborhood_distance) {

					// now add this location to the map in the TQvector element
					if(TQvector[outter].neighborsAndCount.count(it->second.getCategory()) == 0){
						// not present so add it
						TQvector[outter].neighborsAndCount[it->second.getCategory()] = it->second.getTotalCheckIns();
					}else{
						// update the check in count
						int checkIns = TQvector[outter].neighborsAndCount[it->second.getCategory()];
						checkIns += it->second.getTotalCheckIns();
						TQvector[outter].neighborsAndCount[it->second.getCategory()] = checkIns;
					}
				}
			}
		}

	// to check
	int	check = 0;


	// now loop through the TQ vector and calculate the transition quality score
#pragma omp parallel for
	for(int looper = 0; looper < TQvector.size(); looper++){
		double transitionQalityScore = 0.0;
		for(map<string, int>::iterator it = TQvector[looper].neighborsAndCount.begin();
				it != TQvector[looper].neighborsAndCount.end(); ++it){
			if(probMap.count(it->first) == 1){
				transitionQalityScore += (probMap[it->first][0] * (double)it->second);
			}else{
				cout << "Cat: " << it->first << " not found in prob map !!\n";
			}
		}
		TQvector[looper].setTransitionQualityScore(transitionQalityScore);
	}

	// now write stuff to file
	string TQFileName = "./calculated/TQ" + categoryToCheck + ".txt";
	ofstream myfile(TQFileName.c_str());
	if (myfile.is_open()) {
		myfile << "place_id,TQ, lat, lon\n";
		for (int looper = 0; looper != TQvector.size(); looper++) {
			myfile << TQvector[looper].getPlaceId() << ", "
					<< TQvector[looper].getTransitionQualityScore() << ", "
					<< TQvector[looper].getLatitude() << ", "
					<< TQvector[looper].getLongitude() << "\n";
		}
		myfile.close();
	} else {
		cout << "Unable to open file";
	}
}

void calculateIncomingFlow() {

	double neighborhood_distance = 200.0;
	cout << "Calculate incoming flow called \n";

	vector<IncomingCalcHolder> incomingFlowVector;
	for (map<int, LocData>::iterator it = locDataMap.begin();
			it != locDataMap.end(); ++it) {
		int placeId = it->second.getPlaceId();
		IncomingCalcHolder calcTran;
		calcTran.setPlaceId(it->second.getPlaceId());
		calcTran.setLatitude(it->second.getLatitude());
		calcTran.setLongitude(it->second.getLongitude());
		calcTran.setIncominddata(0);
		incomingFlowVector.push_back(calcTran);
	}

#pragma omp parallel for
	for (int outer = 0; outer < incomingFlowVector.size(); outer++) {

		map<int, int> neighborMap;
		int i = omp_get_thread_num();
		for (int inner = 0; inner < incomingFlowVector.size(); inner++) {
			if (incomingFlowVector[outer].getPlaceId()
					== incomingFlowVector[inner].getPlaceId()) {
				continue;
			}
			if (calculateDistance(incomingFlowVector[outer].getLatitude(),
					incomingFlowVector[outer].getLongitude(),
					incomingFlowVector[inner].getLatitude(),
					incomingFlowVector[inner].getLongitude())
					< neighborhood_distance) {
				neighborMap.insert(pair<int, int>(incomingFlowVector[inner].getPlaceId(), 1));
			}
		}

		int flowCounter = 0;

		// loop through all the transition
		for (map<int, int>::iterator it_neighbor = neighborMap.begin(); it_neighbor != neighborMap.end(); ++ it_neighbor) {
			// dest is in the neighborhood
			if (tranMapDestToSource.count(it_neighbor->first) == 1) {
				// loop through all the sources to find the ones outside
				for (map<int, int>::iterator it_dest2src_inner =
						tranMapDestToSource[it_neighbor->first].begin();
						it_dest2src_inner
								!= tranMapDestToSource[it_neighbor->first].end();
						++it_dest2src_inner) {

					if(neighborMap.count(it_dest2src_inner->first) == 0){
						flowCounter += it_dest2src_inner->second;
					}
				}
			}
		}

		incomingFlowVector[outer].setIncominddata(flowCounter);
	}

	ofstream myfile("./calculated/incoming_flow.txt");
	if (myfile.is_open()) {
		myfile << "place_id,Incoming_data, lat, lon\n";
		for (int looper = 0; looper != incomingFlowVector.size(); looper++) {
			myfile << incomingFlowVector[looper].getPlaceId() << ", "
					<< incomingFlowVector[looper].getIncominddata() << ", "
					<< incomingFlowVector[looper].getLatitude() << ", "
					<< incomingFlowVector[looper].getLongitude() << "\n";
		}
		myfile.close();
	} else {
		cout << "Unable to open file";
	}
}

void calculateTransitionDensity(){
	double neighborhood_distance = 200.0;
	cout << "Calculate transition density called \n";

    vector<TransitionDensityCalcHolder> transitionVector;
    for (map<int, LocData>::iterator it = locDataMap.begin();
    			it != locDataMap.end(); ++it) {
         int placeId =  it->second.getPlaceId();
         TransitionDensityCalcHolder transitionDensityCalcHolder;
         transitionDensityCalcHolder.setPlaceId(it->second.getPlaceId());
         transitionDensityCalcHolder.setLatitude(it->second.getLatitude());
         transitionDensityCalcHolder.setLongitude(it->second.getLongitude());
         transitionDensityCalcHolder.setTransitionScore(0);
         transitionVector.push_back(transitionDensityCalcHolder);
      }

#pragma omp parallel for
     for(int outer = 0;outer<transitionVector.size();outer++){
    	 map<int, int> neighborMap;
    	 int transistionScore = 0;
    	 for(int inner =0;inner<transitionVector.size();inner++){
    		 if(transitionVector[outer].getPlaceId() == transitionVector[inner].getPlaceId()){
    		                  continue;
    		 }
    		 if (calculateDistance(transitionVector[outer].getLatitude(),
    				 transitionVector[outer].getLongitude(),
    				 transitionVector[inner].getLatitude(),
    				 transitionVector[inner].getLongitude())
    		 					< neighborhood_distance){
				neighborMap.insert(pair<int, int>(transitionVector[inner].getPlaceId(), 1));
    		 }
    	 }

    	 for(map<int, int>::iterator it = neighborMap.begin(); it != neighborMap.end(); ++it){
    		 if(tranMapSourceToDest.count(it->first) == 1){

    			 // now that we have one of the neighbors iterate through all its transitions
    			 // and see if they also fall in the neighborhood

				for (map<int, int>::iterator it_tranMap =
						tranMapSourceToDest[it->first].begin();
						it_tranMap != tranMapSourceToDest[it->first].end(); ++it_tranMap) {

					// now check if the placeId2 is also inside the neighborhood
					if(neighborMap.count(it_tranMap->first) == 1){
						transistionScore += it_tranMap->second;
					}
				}
    		 }
    	 }
    	 transitionVector[outer].setTransitionScore(transistionScore);
     }

     ofstream myfile("./calculated/transitiondensity.txt");
     if (myfile.is_open()) {
		myfile << "place_id, Transition_density, lat, lon\n";
		for (int looper = 0; looper != transitionVector.size(); looper++) {
			myfile << transitionVector[looper].getPlaceId() << ", "
					<< transitionVector[looper].getTransitionScore() << ", "
					<< transitionVector[looper].getLatitude() << ", "
					<< transitionVector[looper].getLongitude() << "\n";
		}
		myfile.close();
	} else {
		cout << "Unable to open file";
	}
}

void calculateAreaPopularity(){
	double neighborhood_distance = 200.0;
	cout << "Calculate area popularity called \n";

	vector <AreaPopularityCalcHolder> AreaPopVector;
	for (map<int, LocData>::iterator it = locDataMap.begin();
				it != locDataMap.end(); ++it) {
		AreaPopularityCalcHolder areaPopularityCalcHolder;
		areaPopularityCalcHolder.setPlaceId(it->second.getPlaceId());
		areaPopularityCalcHolder.setLatitude(it->second.getLatitude());
		areaPopularityCalcHolder.setLongitude(it->second.getLongitude());
		areaPopularityCalcHolder.setAreaPopularityCount(0);
		areaPopularityCalcHolder.setTotalCheckIns(it->second.getTotalCheckIns());
		AreaPopVector.push_back(areaPopularityCalcHolder);
	}

#pragma omp parallel for
	for (int outter = 0; outter < AreaPopVector.size(); outter++) {
		int totalNeighborhoodCheckins = 0;
		for (int inner = 0; inner < AreaPopVector.size(); inner++) {
			if (AreaPopVector[outter].getPlaceId()
					== AreaPopVector[inner].getPlaceId()) {
				continue;
			}
			if (calculateDistance(AreaPopVector[outter].getLatitude(),
					AreaPopVector[outter].getLongitude(),
					AreaPopVector[inner].getLatitude(),
					AreaPopVector[inner].getLongitude())
					< neighborhood_distance) {
				totalNeighborhoodCheckins += AreaPopVector[inner].getTotalCheckIns();
			}
		}
		AreaPopVector[outter].setAreaPopularityCount(totalNeighborhoodCheckins);
	}

	ofstream myfile("./calculated/area_popularity.txt");
	if (myfile.is_open()) {
		myfile << "place_id, area_pop_count, lat, lon\n";
		for (int looper = 0; looper != AreaPopVector.size(); looper++) {
			myfile << AreaPopVector[looper].getPlaceId() << ", "
					<< AreaPopVector[looper].getAreaPopularityCount() << ", "
					<< AreaPopVector[looper].getLatitude() << ", "
					<< AreaPopVector[looper].getLongitude() << "\n";
		}
		myfile.close();
	} else {
		cout << "Unable to open file";
	}

}

void calculateNeighborsEntropy(){
	double neighborhood_distance = 200.0;
	cout << "Calculate neighbors entropy called \n";

	vector<NeighborsEntCalcHolder> NeighborsEntVector;

	for (map<int, LocData>::iterator it = locDataMap.begin();
			it != locDataMap.end(); ++it) {
		NeighborsEntCalcHolder neighborsEntCalcHolder;
		neighborsEntCalcHolder.setPlaceId(it->second.getPlaceId());
		neighborsEntCalcHolder.setLatitude(it->second.getLatitude());
		neighborsEntCalcHolder.setLongitude(it->second.getLongitude());
		neighborsEntCalcHolder.setCategory(it->second.getCategory());
		neighborsEntCalcHolder.setNeighborEntScore(0.0);
		NeighborsEntVector.push_back(neighborsEntCalcHolder);
	}

#pragma omp parallel for
	for (int outter = 0; outter < NeighborsEntVector.size(); outter++) {

		map<string, double> categoryBasedCountInNeighborhood;
		double totalNeighbors = 0.0;

		for (int inner = 0; inner < NeighborsEntVector.size(); inner++) {
			if (NeighborsEntVector[outter].getPlaceId()
					== NeighborsEntVector[inner].getPlaceId()) {
				continue;
			}

			if (calculateDistance(NeighborsEntVector[outter].getLatitude(),
					NeighborsEntVector[outter].getLongitude(),
					NeighborsEntVector[inner].getLatitude(),
					NeighborsEntVector[inner].getLongitude())
					< neighborhood_distance) {

				if (categoryBasedCountInNeighborhood.count(
						NeighborsEntVector[inner].getCategory()) == 1) {
					double count = categoryBasedCountInNeighborhood[NeighborsEntVector[inner].getCategory()];
					categoryBasedCountInNeighborhood[NeighborsEntVector[inner].getCategory()] = count + 1.0;
				} else {
					categoryBasedCountInNeighborhood[NeighborsEntVector[inner].getCategory()] = 1.0;
				}

				totalNeighbors += 1.0;
			}
		}
		double NeighborEntScore = 0.0;
		for (map<string, double>::iterator it =
				categoryBasedCountInNeighborhood.begin();
				it != categoryBasedCountInNeighborhood.end(); ++it) {

			NeighborEntScore += (it->second / totalNeighbors) * log(it->second / totalNeighbors);
			NeighborsEntVector[outter].setNeighborEntScore(NeighborEntScore);
		}
	}

	ofstream myfile("./calculated/neighbor_entropy.txt");
	if (myfile.is_open()) {
		myfile << "place_id, n_entropty, lat, lon\n";
		for (int looper = 0; looper != NeighborsEntVector.size(); looper++) {
			myfile << NeighborsEntVector[looper].getPlaceId() << ", "
					<< NeighborsEntVector[looper].getNeighborEntScore() << ", "
					<< NeighborsEntVector[looper].getLatitude() << ", "
					<< NeighborsEntVector[looper].getLongitude() << "\n";
		}
		myfile.close();
	} else {
		cout << "Unable to open file";
	}

}

void calculateCompetetiveness(){
	double neighborhood_distance = 200.0;
	cout << "Calculate competitiveness called \n";
	vector<CompetetivenessCalcHolder> competetivenessVector;

	for (map<int, LocData>::iterator it = locDataMap.begin();
			it != locDataMap.end(); ++it) {
		CompetetivenessCalcHolder competetivenessCalcHolder;
		competetivenessCalcHolder.setPlaceId(it->second.getPlaceId());
		competetivenessCalcHolder.setLatitude(it->second.getLatitude());
		competetivenessCalcHolder.setLongitude(it->second.getLongitude());
		competetivenessCalcHolder.setCategory(it->second.getCategory());
		competetivenessCalcHolder.setCompetetiveScore(0.0);
		competetivenessVector.push_back(competetivenessCalcHolder);
	}

#pragma omp parallel for
	for (int outter = 0; outter < competetivenessVector.size(); outter++) {
		double totalNeighbors = 1.0;
		double neighborsOfMyType = 0.0;
		for (int inner = 0; inner < competetivenessVector.size(); inner++) {
			if(calculateDistance(competetivenessVector[outter].getLatitude(),
					competetivenessVector[outter].getLongitude(),
					competetivenessVector[inner].getLatitude(),
					competetivenessVector[inner].getLongitude())
					< neighborhood_distance) {

				if (competetivenessVector[outter].getCategory().compare(
						competetivenessVector[inner].getCategory()) == 0) {
					neighborsOfMyType += 1.0;
				}
				totalNeighbors += 1.0;
			}
		}

		competetivenessVector[outter].setCompetetiveScore(neighborsOfMyType / totalNeighbors);
	}

	ofstream myfile("./calculated/competetiveness.txt");
			if (myfile.is_open()) {
				myfile << "place_id, Competitiveness, lat, lon\n";
				for (int looper = 0; looper != competetivenessVector.size(); looper++) {
					myfile << competetivenessVector[looper].getPlaceId() << ", " << competetivenessVector[looper].getCompetetiveScore()
							<< ", " << competetivenessVector[looper].getLatitude() << ", "
							<< competetivenessVector[looper].getLongitude() << "\n";
				}
				myfile.close();
			} else{
				cout << "Unable to open file";
			}
}

void calculateDensity() {

	double neighborhood_distance = 200.0;
	cout << "Calculate density called\n";
	vector<DensityCalcHolder> densityVector;
	map<int, DensityCalcHolder> densityMap;

	for(map<int, LocData>::iterator it = locDataMap.begin(); it != locDataMap.end(); ++it){
		DensityCalcHolder densityCalcHolder;
		densityCalcHolder.setPlaceId(it->second.getPlaceId());
		densityCalcHolder.setLatitude(it->second.getLatitude());
		densityCalcHolder.setLongitude(it->second.getLongitude());
		densityCalcHolder.setDensity(0);
		densityVector.push_back(densityCalcHolder);
	}

#pragma omp parallel for
	for (int outter = 0; outter < densityVector.size(); outter++) {
		int density =0;
		for (int inner = 0; inner < densityVector.size(); inner++) {
			if (densityVector[outter].getPlaceId()
					== densityVector[inner].getPlaceId()) {
				continue;
			}
			if (calculateDistance(densityVector[outter].getLatitude(),
					densityVector[outter].getLongitude(),
					densityVector[inner].getLatitude(),
					densityVector[inner].getLongitude())
					< neighborhood_distance) {
				density++;
			}
		}
		densityVector[outter].setDensity(density);
	}
	for(int i = 0; i < densityVector.size(); i++){
		densityMap.insert(
					pair<int, DensityCalcHolder>(densityVector[i].getPlaceId(), densityVector[i]));
	}

	ofstream myfile("./calculated/density.txt");
		if (myfile.is_open()) {
			myfile << "place_id, density, lat, lon\n";
			for (std::map<int, DensityCalcHolder>::iterator it = densityMap.begin();
					it != densityMap.end(); ++it) {
				myfile << it->second.getPlaceId() << ", " << it->second.getDensity()
						<< ", " << it->second.getLatitude() << ", "
						<< it->second.getLongitude() << "\n";
			}
			myfile.close();
		} else{
			cout << "Unable to open file";
		}
}

void calcDefaultRank() {

	double neighborhood_distance = 200.0;
	cout << "Calculate default rank called \n";

	ofstream myfile("./calculated/default.txt");
	if (myfile.is_open()) {
		myfile << "check_ins, place_id, category, lat, lon\n";
		for (map<int, LocData>::iterator it = locDataMap.begin();
				it != locDataMap.end(); ++it) {
			myfile << it->second.getTotalCheckIns() << ", "
					<< it->second.getPlaceId() << ", "
					<< it->second.getCategory() << ", "
					<< it->second.getLatitude() << ", "
					<< it->second.getLongitude() << "\n";
		}
		myfile.close();
	} else {
		cout << "Unable to open default rank file";
	}

}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
	double dlat = degToRads(lat1 - lat2);
	double dlon = degToRads(lon1 - lon2);
	double a = pow((sin(dlat / 2.0)), 2)
			+ (cos(degToRads(lat1)) * cos(degToRads(lat2))
					* pow(sin(dlon / 2.0), 2));
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	c *= EARTH_RADIUS;
	return c * 1000.0;
}

double degToRads(double degs) {
	return degs * (M_PI / 180.0);
}
