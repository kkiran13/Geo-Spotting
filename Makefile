all:
	g++ -fopenmp GeoSpotting.cpp -o GeoSpotting
	g++ CalcScores.cpp -o CalcScores
