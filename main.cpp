#include <iostream>
#include <sstream>
#include <algorithm>

#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

int main(int argc, char **argv)
{
	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " <repo> <field> <docno>" << endl;
		return EXIT_FAILURE;
	}

	cerr << "Built with " << INDRI_DISTRIBUTION << endl;

	string repository_name = argv[1];
	string field = argv[2];
	string docno = argv[3];

	if (!indri::collection::Repository::exists(repository_name)) {
		cerr << "Repository " << repository_name << " doesn't exist." << endl;
		return EXIT_FAILURE;
	}

	indri::collection::Repository repo;
	repo.openRead(repository_name);

	indri::collection::CompressedCollection *collection = repo.collection();
	vector<lemur::api::DOCID_T> ids = collection->retrieveIDByMetadatum("docno", docno);
	if (ids.empty()) {
		repo.close();
		cerr << "Document " << docno << " not found." << endl;
		return EXIT_FAILURE;
	} else if (ids.size() > 1) {
		cerr << "Warning: Retrieved " << ids.size() << " ids for " << docno << "." << endl;
	}

	auto id = ids[0];
	indri::collection::Repository::index_state state = repo.indexes();
	indri::index::Index *index = state->at(0);
	indri::index::DocExtentListIterator *iter = index->fieldListIterator(field);
	iter->startIteration();

	bool result = iter->nextEntry(id);
	if (!result) {
		repo.close();
		cerr << "Can't find " << field << " for " << docno << "." << endl;
		return EXIT_FAILURE;
	}

	indri::index::DocExtentListIterator::DocumentExtentData *data = iter->currentEntry();
	const indri::index::TermList *list = index->termList(data->document);;
	auto terms = list->terms();
	string value;
	for (auto& extent : data->extents) {
		for (size_t i = extent.begin; i < extent.end; i++) {
			value += index->term(terms[i]) + " ";
		}
	}
	repo.close();

	// Left and right whitespace
	trim(value);

	// remove apostrophes
	value.erase(std::remove_if(value.begin(), value.end(),
				   [](char c) { return c == '\'' || c == '\"';}),
		    value.end());

	// remove line break and non alnum
	std::replace_if(value.begin(), value.end(),
			[](char c) { return c == '\n' || !isalnum(c); }, ' ');

	cout << value;

	return EXIT_SUCCESS;
}
