
#include <iostream>
#include <sstream>

#include "indri/Repository.hpp"
#include "indri/CompressedCollection.hpp"

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "usage: " << argv[0]
			  << " <repo> <field>" << std::endl;
		return EXIT_FAILURE;
	}

	std::string repository_name = argv[1];
	std::string field_name = argv[2];

	indri::collection::Repository repo;
	repo.openRead(repository_name);
	indri::collection::Repository::index_state state = repo.indexes();
	const auto &index = (*state)[0];

	if (!index->field(field_name)) {
		std::cerr << "the field '" << field_name << "' does not exist." << std::endl;
		std::cerr << "available fields are:" << std::endl;
		int id = 1;
		while ("" != index->field(id)) {
			std::cerr << "  " << index->field(id++) << std::endl;
		}
		return EXIT_FAILURE;
	}

	std::cerr << "extracting fields from indri index." << std::endl;
	std::vector<std::string> document_names;

	indri::collection::CompressedCollection *collection = repo.collection();
	indri::index::DocExtentListIterator *iter = index->fieldListIterator(field_name);

	size_t id = index->documentBase();
	const size_t total_docs = index->fieldDocumentCount(field_name);
	iter->startIteration();
	while (!iter->finished()) {
		indri::index::DocExtentListIterator::DocumentExtentData *data = iter->currentEntry();
		const indri::index::TermList *list = index->termList(data->document);
		std::string doc_name = collection->retrieveMetadatum(data->document, "docno");
		auto terms = list->terms();

		std::cout << "<DOC>" << std::endl;
		std::cout << "<DOCNO>" << doc_name << "</DOCNO>" << std::endl;
		std::cout << "<TEXT>" << std::endl;
		for (auto& extent : data->extents) {
			for (size_t i = extent.begin; i < extent.end; i++) {
				std::cout << index->term(terms[i]) << " ";
			}
		}
		std::cout << std::endl << "</TEXT>" << std::endl << "</DOC>" << std::endl;

		if (id % 500 == 0 || id == index->documentBase() || size_t(id) == total_docs) {
			static size_t last_len = 0;
			std::ostringstream oss;
			double progress = double(id) / double(total_docs);
			progress *= 100;
			std::cerr << std::string(last_len, '\b');
			oss.str("");
			oss << "documents dumped: " << id << " (" << int(progress) << "%)";
			last_len = oss.str().size();
			std::cerr << oss.str();
		}

		delete list;
		iter->nextEntry();
		id++;
	}
	std::cerr << std::endl;
}
