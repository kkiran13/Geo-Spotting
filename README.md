# Geo-Spotting
Finding Optimal Location of Retail Store Placement using Online Location Based Data

Data set used is from Foursquare. Features like place ID, position (latitude and longitude), category of store, number of checkins from users are present.
Task is to find the optimal location for a particular category of store to help maximize the business.

Running:

1) use "make" command to compile the code into binaries

2) ./GeoSpotting <path_to_new_york_loc_file> <path_to_new_york_placenet_file> <category>

Category can be for example: _Coffee_Shop. This would give the intermediate results in files namely:
default_score_file, density_score_file, neighbor_entropy_score_file, competitiveness_score_file, area_popularity_score_file
transition_density_score_file, incoming_flow_score_file, transition_quality_score_file

3) ./CalcScores 10 calculated/default.txt calculated/density.txt calculated/neighbor_entropy.txt calculated/competetiveness.txt calculated/area_popularity.txt calculated/transitiondensity.txt calculated/incoming_flow.txt calculated/TQ_Coffee_Shop.txt _Coffee_Shop
This would give different scores for Coffee_Shops based on:
Density, Entropy, competitiveness , area popularity , transition density, incoming Flow, transition quality
