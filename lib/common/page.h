//
//  page.h
//  search_engine
//
//  Created by Felipe Moraes on 03/20/15.
//
//


#ifndef __search_engine__page__
#define __search_engine__page__

#include <string>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;

class Page {
  public:

    Page(string url, string httpResponse);

    string get_text();
    string get_url();
    string get_title();
    string get_keywords();
    string get_description();
    string get_content_type();
    map<string, string> get_links();

  private:

    void parse(const string& html);
    bool remove_header(string& str);
    string url_;
    string text_;
    string title_;
    string keywords_;
    string description_;
    string content_type_;
    map<string, string> links_;
	
};

inline string Page::get_text(){
    return text_;
}

inline string Page::get_url(){
    return url_;
}

inline string Page::get_title(){
    return title_;
}

inline string Page::get_keywords(){
    return keywords_;
}

inline string Page::get_description(){
    return description_;
}

inline string Page::get_content_type(){
    return content_type_;
}

inline map<string, string> Page::get_links(){
    return links_;
}

#endif /* defined(__search_engine__page__) */