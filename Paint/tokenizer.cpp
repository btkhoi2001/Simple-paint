#include "tokenizer.h"

vector<string> Tokenizer::spilit(string haystack, string needle) {
	vector <string> result;
	int start_pos = 0;
	size_t found_pos = 0;

	while (true) {
		found_pos = haystack.find(needle, start_pos);

		if (found_pos != string::npos) {
			string token = haystack.substr(start_pos, found_pos - start_pos);
			result.push_back(token);
			start_pos = found_pos + needle.length();
		}
		else {
			string token = haystack.substr(start_pos, haystack.length() - start_pos);
			result.push_back(token);
			break;
		}
	}

	return result;
}
