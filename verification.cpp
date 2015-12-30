#include <stdio.h>
#include <vector>
#include <string>
#include <utility>

using namespace std;

typedef pair<size_t, size_t> hit;


vector<size_t> find_potetntial_matches(string& T, string& P, vector<hit> hit_list, size_t t)
{

	vector<size_t> potential_matches;


	size_t array_size = T.size()+P.size()+1;

	size_t pattern_index_array[array_size];


	potential_matches.push_back(0);
	potential_matches.push_back(2);
	potential_matches.push_back(3);
	potential_matches.push_back(20);

	return potential_matches;
}


void verify_potential_matches(string& T, string& P, vector<size_t> potential_matches, size_t k)
{

	size_t length = P.size();
	size_t nbr_of_samples = potential_matches.size();
	size_t nbr_of_matches = 0;
	vector<size_t> matches_list;

	for(size_t i = 0 ; i < nbr_of_samples ; i++){

		size_t miss_count = 0, j = 0;
		size_t matching = 0;
		size_t sample_start = potential_matches[i];

		do{

			if(T[sample_start+j] != P[j]) miss_count++;

			j++;

		}while((miss_count <= k) && (j < length));

		if(miss_count <= k){
			nbr_of_matches++;
			matches_list.push_back(sample_start);
		}

	}
	
	printf("%d\n",nbr_of_matches);

	return;
}

int main(){
	/*-----------------------*/
	string T = "ATCATACACGATCGATCAGCATCAAACG";
	string P = "ATCAAAC";
	vector<hit> hit_list;
	size_t q_span=4;
	size_t t=2;
	size_t k=1;
	/*-----------------------*/

	vector<size_t> potential_matches;

	potential_matches=find_potetntial_matches(T,P,hit_list,t);

	verify_potential_matches(T,P,potential_matches,k);


	return 0;
}