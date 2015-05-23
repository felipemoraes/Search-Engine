//
//  vector_space_model.cc
//  search_engine
//
//  Created by Felipe Moraes on 5/16/15.
//
//

#include "vector_space_model.h"
#include <math.h>


using namespace boost;
using namespace std::placeholders;


VectorSpaceModel::VectorSpaceModel(IndexFile* index, Vocabulary *vocabulary, DocRepository* doc_repository)  : RankingModel(index,vocabulary,doc_repository) {
   
}




vector<Hit>* VectorSpaceModel::search(string query){
    
    unordered_map<unsigned,float>* accumulators = new unordered_map<unsigned,float>();
    remove_accents(query);
    transform(query.begin(), query.end(), query.begin(),::tolower);
    tokenizer<> tokens(query);
    // aggregate terms by positions
    
    for(auto token = tokens.begin(); token!=tokens.end();++token){
        // check if term is not a stopword
       // if (stopwords_.find(*token) != stopwords_.end()) {
       //     continue;
       // }
        
        int term_id = vocabulary_->get_term_id(*token);
        long seek = vocabulary_->get_seek(term_id);
        Term term = index_->read(seek);

        
        if (term_id != -1) {
            float term_weight = 1 + log(vocabulary_->get_frequence(*token)/(float)term.frequency_);
            for (auto it = term.docs_->begin(); it != term.docs_->end();++it) {
                if (accumulators->find(it->doc_id_) != accumulators->end()) {
                    (*accumulators)[it->doc_id_] += log(1 + it->frequency_)*term_weight;
                } else {
                    accumulators->insert(make_pair(it->doc_id_, 0));
                }
            }
            
            for (auto it = accumulators->begin(); it != accumulators->end(); ++it) {
                DocumentInfo doc = doc_repository_->find(it->first);
                if (doc.doc_id_ != it->first + 1) {
                    it->second /= doc.length_;
                }
            }
        }
    }
    vector<Hit>* hits = new vector<Hit>();
    for (auto item: *accumulators) {
        Hit hit(doc_repository_->find(item.first),item.second);
        hits->push_back(hit);
    }
    sort(hits->begin(),hits->end());
    return hits;
}