//
//  reducer.cc
//  search_engine
//
//  Created by Felipe Moraes on 3/28/15.
//
//

#include "reducer.h"
#include <queue>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;


Reducer::Reducer(int buffer_size, vector<File* > &runs){
    runs_ = runs;
    buffer_size_ = buffer_size;
}

void Reducer::merge(){
    
    int block_number = 0;
    int buffer_length = 0;
    TermOccurrence* buffer = new TermOccurrence[buffer_size_];
    stringstream file_name;
    priority_queue< TermOccurrence, vector<TermOccurrence>, greater<TermOccurrence> > heap;
    string directory = "/Users/felipemoraes/Developer/search-engine/data/tmp_files";
    file_name << directory << "/mergedfile" << block_number;
    merged_ = new File(file_name.str());
    
    map<TermOccurrence, int> indices;
    
    vector<File*>::iterator it;
    
    for(it = runs_.begin(); it != runs_.end(); it++){
        File* run = *it;
        run->reopen();
        
        TermOccurrence term = run->read();
        heap.push(term);
        indices[term] = it - runs_.begin();
        
        cout << "Run "<< it - runs_.begin() << " size: " << run->get_size() << endl;
    }
    while( !heap.empty() ){
        TermOccurrence top = heap.top();
        
        buffer[buffer_length] = top;
        heap.pop();
        buffer_length++;
        if(buffer_length >= buffer_size_){
            merged_->write_block(buffer, buffer_size_);
            buffer_length = 0;
        }
        
        int index = indices[top];
        indices.erase(top);
        if(runs_[index]->has_next()){
            TermOccurrence head = runs_[index]->read();
            heap.push(head);
            indices[head] = index;
        }
    }
    
    for(it = runs_.begin(); it != runs_.end(); it++){
        File* run = *it;
        run->delete_file();
    }
    
    if(buffer_length > 0){
        merged_->write_block(buffer, buffer_length);
        buffer_length = 0;
    }
    cout << "Finished merging " << directory << "/mergedfile" << " with " << merged_->get_size() << " entries." << endl;
    merged_->close();
}

void Reducer::reduce(){
    merged_->reopen();
    int term_id = -1;
    Term aggr_term;
    IndexFile* index = new IndexFile("/Users/felipemoraes/Developer/search-engine/data/index");
    aggr_term.docs_ = new vector<Doc>();
    while (merged_->has_next()) {
        TermOccurrence term = merged_->read();
        if(term_id != term.term_id_){
            if (term_id != -1) {
                index->write(aggr_term);
                cout << aggr_term.term_id_ << endl;
            }
            term_id = term.term_id_;
            aggr_term.term_id_ = term_id;
            aggr_term.frequency_ = term.frequency_;
            aggr_term.docs_->clear();
        }
        aggr_term.frequency_ += term.frequency_;
        Doc doc;
        doc.frequency_ = term.frequency_;
        doc.doc_id_ = term.doc_id_;
        vector<int>* positions = term.get_positions();
        vector<int>::iterator it;
        doc.positions_ = new vector<int>();
        for (it = positions->begin(); it != positions->end(); it++) {
            doc.positions_->push_back(*it);
        }
        aggr_term.docs_->push_back(doc);
    }
    index->write(aggr_term);
    cout << aggr_term.term_id_ << endl;
    merged_->delete_file();
    index->close();
}