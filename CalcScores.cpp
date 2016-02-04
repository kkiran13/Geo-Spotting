#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <math.h>

using namespace std;

#define EARTH_RADIUS 6371;

string default_file_name;
string density_file_name;
string entropy_file_name;
string competitiveness_file_name;
string areapopularity_file_name;
string transitiondensity_file_name;
string incomingflow_file_name;
string TQ_file_name;

double DensityDGCval_wild = 0.0;
double DensityDGCval_list = 0.0;
double EntropyDGCval_wild = 0.0;
double EntropyDGCval_list = 0.0;
double CompetetivenessDGCval = 0.0;
double AreaPopularityDGCval = 0.0;
double TransitionDGCval = 0.0;
double IncomingFlowDGCval = 0.0;
double TQ_DGCval = 0.0;

map<int, vector<double> > placeId_lat_lon_map;
map<int, double> topKWithRelevanceScores;

// NOTE : the space before the cat is very imp DP NOT REMOVE
string CategoryToCheck = " _Coffee_Shop";

void calculateNDGC(int);
void doDensityCalcWithGivenList(int);
void doEntropyCalcWithGivenList(int);
void doCompetitivenessCalcWithGivenList(int);
void doAreaPopularityCalcWithGivenList(int);
void doTransitionDensityCalcWithGivenList(int);
void doIncomingFlowCalcWithGivenList(int);
void doTransitionQualityCalcWithGivenList(int);
double degToRads(double degs);
double calculateDistance(double lat1, double lon1, double lat2, double lon2);


int main(int argc, char** argv){

	if(argc != 11){
		cout << "This program takes in 10 parameters in this order: \n";
		cout << "<int_value_of_K> \n";
		cout << "<default_score_file> \n";
		cout << "<density_score_file> \n";
		cout << "<neighbor_entropy_score_file> \n";
		cout << "<competitiveness_score_file> \n";
		cout << "<area_popularity_score_file> \n";
		cout << "<transition_density_score_file> \n";
		cout << "<incoming_flow_score_file> \n";
		cout << "<transition_quality_score_file> \n";
		cout << "<category_to_calculate_scores_for> \n";
		exit(0);
	}
	int k = 10;
	k = atoi(argv[1]);
	default_file_name = argv[2];
	density_file_name = argv[3];
	entropy_file_name = argv[4];
	competitiveness_file_name = argv[5];
	areapopularity_file_name = argv[6];
	transitiondensity_file_name = argv[7];
	incomingflow_file_name = argv[8];
	TQ_file_name = argv[9];
	CategoryToCheck = argv[10];
	CategoryToCheck = " " + CategoryToCheck;
	cout << "Doing calculations for: " << CategoryToCheck << "\n";
	calculateNDGC(k);
}

// to calculate NDGC@k for starbucks
void calculateNDGC(int k){

	string line;
	// readin ranking based on check ins
	map<int, vector< int> , std::greater<int> >defaultRankMap;
	ifstream defaultFile(default_file_name.c_str());
	if (defaultFile.is_open()) {
		while (getline(defaultFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int checkins = atoi(token);
			token = strtok(NULL, ",");
			int placeID = atoi(token);
			token = strtok(NULL, ",");
			string cat = token;
			token = strtok(NULL, ", ");
			double lat = atof(token);
			token = strtok(NULL, ", ");
			double lon = atof(token);

			if(cat.compare(CategoryToCheck) == 0){
			//	cout << checkins << " " << placeID << " " << cat << "\n";
				if(defaultRankMap.count(checkins) == 0){
					vector< int> placeIDVector;
					placeIDVector.push_back(placeID);
					defaultRankMap.insert(pair<int , vector<int> >(checkins, placeIDVector));
				}else{
					defaultRankMap[checkins].push_back(placeID);
				}

				// add it to the map of coffee_shops
				vector<double> lat_lon;
				lat_lon.push_back(lat);
				lat_lon.push_back(lon);
				placeId_lat_lon_map.insert(pair<int, vector<double> >(placeID, lat_lon));
			}
		}
	}

	//cout << "place ID cat map size: " << placeId_lat_lon_map.size() << "\n";
	//cout << "default rank map size: " << defaultRankMap.size() << "\n";

	// calculate the relevance scores
	int rank = 1;
	int counter = 0;

	//int size = k;
	//for(map<int, vector<int> >::iterator it = defaultRankMap.begin(); it != defaultRankMap.end() && counter < size; ++it){
	int size = defaultRankMap.size();
	for(map<int, vector<int> >::iterator it = defaultRankMap.begin(); it != defaultRankMap.end(); ++it){
		for(int looper = 0; looper < it->second.size() ; looper++){
			double rel = 0.0;
			rel = (double)(size - rank + 1) / (double)size;
			topKWithRelevanceScores.insert(pair<int, double>(it->second[looper], rel));
			//cout << rank << " " << it->first << " " << rel << " " << it->second[looper] << "\n";
		}
		counter++;
		rank++;
	}

	//---- Density -----//
	doDensityCalcWithGivenList(k);

	//---- Entropy ----//
	doEntropyCalcWithGivenList(k);

	//---- Competitiveness ----//
	doCompetitivenessCalcWithGivenList(k);

	//---- Area Popularity ----//
	doAreaPopularityCalcWithGivenList(k);

	//---- Transition Density ----//
	doTransitionDensityCalcWithGivenList(k);

	//---- Incoming flow ----//
	doIncomingFlowCalcWithGivenList(k);

	//---- Transition Quality ----//
	doTransitionQualityCalcWithGivenList(k);

	cout << "Doing IDGC calculations \n";

	// caluclate IDGC
	counter = 0;
	rank = 1;
	double IDGCval = 0.0;
	for (map<int, vector<int> >::iterator it = defaultRankMap.begin();
			it != defaultRankMap.end() && counter < k; ++it) {
		for(int looper = 0; looper < it->second.size(); looper++){
			double rel = 0.0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}
			double temp = pow(2, rel);
			IDGCval += ((temp - 1) / log2(rank + 1));
			//cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << IDGCval << "\n";
		}
		counter++;
		rank++;
	}

	cout << "\nIDGC = " << IDGCval << "\n";

	// now normalize it
	//cout << "DGC for density list = " << DensityDGCval_list << "\n";
	cout << "NDGC density list = " << (DensityDGCval_list / IDGCval) << "\n";

	//cout << "DGC for entropy list = " << EntropyDGCval_list << "\n";
	cout << "NDGC entropy list = " << (EntropyDGCval_list / IDGCval) << "\n";

	//cout << "DGC for competitiveness list = " << CompetetivenessDGCval << "\n";
	cout << "NDGC competitiveness list = " << (CompetetivenessDGCval / IDGCval) << "\n";

	//cout << "DGC for area popularity list = " << AreaPopularityDGCval << "\n";
	cout << "NDGC area popularity list = " << (AreaPopularityDGCval / IDGCval) << "\n";

	//cout << "DGC for transition density list = " << TransitionDGCval << "\n";
	cout << "NDGC transition density list = " << (TransitionDGCval / IDGCval) << "\n";

	//cout << "DGC for incoming flow list = " << IncomingFlowDGCval << "\n";
	cout << "NDGC incoming flow list = " << (IncomingFlowDGCval / IDGCval) << "\n";

	//cout << "DGC for transition quality list = " << IncomingFlowDGCval << "\n";
	cout << "NDGC transition quality list = " << (TQ_DGCval / IDGCval) << "\n";
}

void doTransitionQualityCalcWithGivenList(int k) {

	// read entropy file
	map<double, vector<int>, std::greater<double> > TQRankMap;
	ifstream TQFile(TQ_file_name.c_str());

	string line;
	if (TQFile.is_open()) {
		while (getline(TQFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);

			token = strtok(NULL, ",");
			double transition_quality = atof(token);

			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (TQRankMap.count(transition_quality) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					TQRankMap.insert(
							pair<double, vector<int> >(transition_quality,
									placeIDVector));
				} else {
					TQRankMap[transition_quality].push_back(placeId);
				}
			}
		}
	}

	// calculate DGC for transition quality
	cout << "Doing TQ Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<double, vector<int> >::iterator it = TQRankMap.begin();
			it != TQRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			int found = 0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			TQ_DGCval += ((temp - 1) / log2(rank + 1));
			//cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << TQ_DGCval << "\n";

			inner_rank++;
			counter++;
			if (counter == k) {
				break;
			}
		}
		rank = inner_rank;
	}

}

void doIncomingFlowCalcWithGivenList(int k) {

	// read incoming flow file
	map<int, vector<int>, std::greater<int> > incomingFlowRankMap;
	ifstream incomingFlowFile(
			incomingflow_file_name.c_str());

	string line;
	if (incomingFlowFile.is_open()) {
		while (getline(incomingFlowFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);

			token = strtok(NULL, ",");
			int inFlow = atoi(token);

			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (incomingFlowRankMap.count(inFlow) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					incomingFlowRankMap.insert(
							pair<int, vector<int> >(inFlow,
									placeIDVector));
				} else {
					incomingFlowRankMap[inFlow].push_back(placeId);
				}
			}
		}
	}

	// calculate DGC for in Flow
	cout << "Doing in Flow Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<int, vector<int> >::iterator it = incomingFlowRankMap.begin();
			it != incomingFlowRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			IncomingFlowDGCval += ((temp - 1) / log2(rank + 1));
	//		cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << IncomingFlowDGCval << "\n";

			inner_rank++;
			counter++;
			if (counter == k) {
				break;
			}
		}
		rank = inner_rank;
	}

}

void doTransitionDensityCalcWithGivenList(int k) {

	// read transition density file
	map<int, vector<int>, std::greater<int> > transitionDensityRankMap;
	ifstream transitionDensityFile(transitiondensity_file_name.c_str());

	string line;
	if (transitionDensityFile.is_open()) {
		while (getline(transitionDensityFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);

			token = strtok(NULL, ",");
			int transitionDensity = atoi(token);

			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (transitionDensityRankMap.count(transitionDensity) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					transitionDensityRankMap.insert(
							pair<int, vector<int> >(transitionDensity,
									placeIDVector));
				} else {
					transitionDensityRankMap[transitionDensity].push_back(placeId);
				}
			}
		}
	}

	// calculate DGC for transition density
	cout << "Doing transition density Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<int, vector<int> >::iterator it = transitionDensityRankMap.begin();
			it != transitionDensityRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			TransitionDGCval += ((temp - 1) / log2(rank + 1));
		//	cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << TransitionDGCval << "\n";

			inner_rank++;
			counter++;
			if (counter == k) {
				break;
			}
		}
		rank = inner_rank;
	}

}

void doAreaPopularityCalcWithGivenList(int k) {

	// read area popularity file
	map<int, vector<int>, std::greater<int> > areaPopularityRankMap;
	ifstream areaPopularityFile(areapopularity_file_name.c_str());

	string line;
	if (areaPopularityFile.is_open()) {
		while (getline(areaPopularityFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);

			token = strtok(NULL, ",");
			double areaPopularity = atoi(token);

			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (areaPopularityRankMap.count(areaPopularity) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					areaPopularityRankMap.insert(
							pair<int, vector<int> >(areaPopularity,
									placeIDVector));
				} else {
					areaPopularityRankMap[areaPopularity].push_back(placeId);
				}
			}
		}
	}

	// calculate DGC for area popularity
	cout << "Doing area popularity Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<int, vector<int> >::iterator it = areaPopularityRankMap.begin();
			it != areaPopularityRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			AreaPopularityDGCval += ((temp - 1) / log2(rank + 1));
			//cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << AreaPopularityDGCval << "\n";

			inner_rank++;
			counter++;
			if (counter == k) {
				break;
			}
		}
		rank = inner_rank;
	}
}

void doCompetitivenessCalcWithGivenList(int k) {

	// read competetiveness file
	map<double, vector<int>, std::greater<double> > competitvenessRankMap;
	ifstream competitivenessFile(competitiveness_file_name.c_str());

	string line;
	if (competitivenessFile.is_open()) {
		while (getline(competitivenessFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);

			token = strtok(NULL, ",");
			double competetiveness = atof(token);
			// the less competetive the place the better so multiply with -1
			if (competetiveness != 0.0) {
				competetiveness = -1 * competetiveness;
			}
			//cout << competetiveness << "\n";
			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (competitvenessRankMap.count(competetiveness) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					competitvenessRankMap.insert(
							pair<double, vector<int> >(competetiveness,
									placeIDVector));
				} else {
					competitvenessRankMap[competetiveness].push_back(placeId);
				}
			}
		}
	}

	// calculate DGC for competitiveness
	cout << "Doing competitiveness Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<double, vector<int> >::iterator it = competitvenessRankMap.begin();
			it != competitvenessRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			int found = 0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			CompetetivenessDGCval += ((temp - 1) / log2(rank + 1));
			//cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << CompetetivenessDGCval << "\n";

			inner_rank++;
			counter++;
			if (counter == k) {
				break;
			}
		}
		rank = inner_rank;
	}

}

void doEntropyCalcWithGivenList(int k){

	// read entropy file
	map<double, vector<int>, std::greater<double> > entropyRankMap;
	ifstream entropyFile(entropy_file_name.c_str());

	string line;
	if (entropyFile.is_open()) {
		while (getline(entropyFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);

			token = strtok(NULL, ",");
			double neighbors_entropy = atof(token);
			// to take care of -ve in file due to log
			if (neighbors_entropy != 0.0) {
				neighbors_entropy = -1 * neighbors_entropy;
			}
			//cout << neighbors_entropy << "\n";
			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (entropyRankMap.count(neighbors_entropy) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					entropyRankMap.insert(
							pair<double, vector<int> >(neighbors_entropy,
									placeIDVector));
				} else {
					entropyRankMap[neighbors_entropy].push_back(placeId);
				}
			}
		}
	}

	// calculate DGC for entropy
	cout << "Doing Entropy Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<double, vector<int> >::iterator it = entropyRankMap.begin();
			it != entropyRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			int found = 0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			EntropyDGCval_list += ((temp - 1) / log2(rank + 1));
		//	cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << EntropyDGCval_list << "\n";

			inner_rank++;
			counter++;
			if (counter == k) {
				break;
			}
		}
		rank = inner_rank;
	}

}

void doDensityCalcWithGivenList(int k){

	// read density file
	map<int, vector<int>, std::greater<int> > densityRankMap;
	ifstream densityFile(density_file_name.c_str());

	string line;
	if (densityFile.is_open()) {
		while (getline(densityFile, line)) {

			char* token = strtok(const_cast<char*>(line.c_str()), ",");
			int placeId = atoi(token);
			token = strtok(NULL, ",");
			int density = atoi(token);

			if (topKWithRelevanceScores.count(placeId) == 1) {
				if (densityRankMap.count(density) == 0) {
					vector<int> placeIDVector;
					placeIDVector.push_back(placeId);
					densityRankMap.insert(
							pair<int, vector<int> >(density, placeIDVector));
				} else {
					densityRankMap[density].push_back(placeId);
				}
			}
		}
	}

	//cout << "density rank map size: " << densityRankMap.size() << "\n";

	// calculate DGC for density
	cout << "Doing Density Calculations \n";

	int counter = 0;
	int rank = 1;
	int inner_rank = 1;

	for (map<int, vector<int> >::iterator it = densityRankMap.begin();
			it != densityRankMap.end() && counter < k; ++it) {
		for (int looper = 0; looper < it->second.size(); looper++) {
			double rel = 0.0;
			int found = 0;
			if (topKWithRelevanceScores.count(it->second[looper]) == 0) {
				rel = 0.0;
			} else {
				rel = topKWithRelevanceScores[it->second[looper]];
			}

			double temp = pow(2, rel);
			DensityDGCval_list += ((temp - 1) / log2(rank + 1));
		//	cout << counter << "\t" << it->second[looper] << "\t" << rel << "\t" << DensityDGCval_list << "\n";

			inner_rank++;
			counter++;
			if(counter == k){
				break;
			}
		}
		rank = inner_rank;
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
