
#include <iostream>
#include <sstream>

#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;

int main(int argc, char **argv)
{
	if (argc != 4) {
		cerr << "Usage: " << argv[0] << " <repo> <docno> <field>" << endl;
		return EXIT_FAILURE;
	}

	string repository_name = argv[1];
	string docno = argv[2];
	string field = argv[3];

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
	}

	string field_value = collection->retrieveMetadatum(ids[0], field);

	cout << field_value << endl;

	repo.close();

	return EXIT_SUCCESS;
}
