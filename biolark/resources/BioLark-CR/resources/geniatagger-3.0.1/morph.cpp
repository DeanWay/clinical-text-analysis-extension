#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

using namespace std;

struct MorphDic
{
  map<string, string> verbex;
  map<string, string> nounex;
  map<string, string> advex;
  map<string, string> adjex;
  set<string> noundic;
  set<string> verbdic;
  set<string> adjdic;
  MorphDic() {}
  bool Init(const string & path) {
//    cerr << "loading morphdic...";
//    cerr << path;
    int load = LoadEx(path + "/morphdic/noun.exc", nounex);
    load += LoadEx(path + "/morphdic/verb.exc", verbex);
    load += LoadEx(path + "/morphdic/adj.exc", adjex);
    load += LoadEx(path + "/morphdic/adv.exc", advex);
    load += LoadIdx(path + "/morphdic/noun.dic", noundic);
    load += LoadIdx(path + "/morphdic/verb.dic", verbdic);
    load += LoadIdx(path + "/morphdic/adj.dic", adjdic);
    
    if (load != 0) {
      return false;
    }
    
    return true;
    
//    cerr << "done." << endl;
  }
  int LoadEx(const string & filename, map<string, string> & exmap) {
    ifstream ifile(filename.c_str());
    if (!ifile) {
      return 1;
//      cerr << "error: cannot open " << filename << endl;
//      exit(1);
    }
    string line;
    while (getline(ifile, line)) {
      istringstream is(line);
      string org, base;
      is >> org >> base;
      exmap[org] = base;
      string org2(org), base2(base);
      org2[0] = toupper(org[0]);
      base2[0] = toupper(base[0]);
      exmap[org2] = base2;
    }
    
    return 0;
  }
  int LoadIdx(const string & filename, set<string> & dic) {
    ifstream ifile(filename.c_str());
    if (!ifile) {
//      cerr << "error: cannot open " << filename << endl;
//     exit(1);
      return 1;
    }
    string line;
    while (getline(ifile, line)) {
      if (line[0] == ' ') continue;
      istringstream is(line);
      string base;
      is >> base;
      dic.insert(base);
      string base2(base);
      base2[0] = toupper(base[0]);
      dic.insert(base2);
    }
    
    return 0;
  }
  bool LookUpDicNoun(const string & s) {
    return noundic.find(s) != noundic.end();
  }
  bool LookUpDicVerb(const string & s) {
    return verbdic.find(s) != verbdic.end();
  }
  bool LookUpDicAdj(const string & s) {
    return adjdic.find(s) != adjdic.end();
  }
  string BaseFormNoun(const string & s) {
    map<string, string>::const_iterator i = nounex.find(s);
    if (i == nounex.end()) return "";
    return i->second;
  }
  string BaseFormVerb(const string & s) {
    map<string, string>::const_iterator i = verbex.find(s);
    if (i == verbex.end()) return "";
    return i->second;
  }
  string BaseFormAdj(const string & s) {
    map<string, string>::const_iterator i = adjex.find(s);
    if (i == adjex.end()) return "";
    return i->second;
  }
  string BaseFormAdv(const string & s) {
    map<string, string>::const_iterator i = advex.find(s);
    if (i == advex.end()) return "";
    return i->second;
  }
};

MorphDic morphdic;

static string base_form_noun(const string & s)
{
  string ex = morphdic.BaseFormNoun(s);
  if (ex != "") return ex;

  int len = s.size();
  if (len > 1) {
    string suf1 = s.substr(len - 1);
    if (suf1 == "s") {
      if (morphdic.LookUpDicNoun(s.substr(0, len - 1))) return s.substr(0, len - 1);
      //      if (morphdic.LookUpDicVerb(s.substr(0, len - 1))) return s.substr(0, len - 1);
    }
  }
  if (len > 4) {
    string suf4 = s.substr(len - 4);
    if (suf4 == "ches") return s.substr(0, len - 4) + "ch";
    if (suf4 == "shes") return s.substr(0, len - 4) + "sh";
  }
  if (len > 3) {
    string suf3 = s.substr(len - 3);
    if (suf3 == "ses") return s.substr(0, len - 3) + "s";
    if (suf3 == "xes") return s.substr(0, len - 3) + "x";
    if (suf3 == "zes") return s.substr(0, len - 3) + "z";
    if (suf3 == "men") return s.substr(0, len - 3) + "man";
    if (suf3 == "ies") return s.substr(0, len - 3) + "y";
  }
  if (len > 1) {
    string suf1 = s.substr(len - 1);
    if (suf1 == "s") return s.substr(0, len - 1);
  }
  return s;
}

static string base_form_verb(const string & s)
{
  string ex = morphdic.BaseFormVerb(s);
  if (ex != "") return ex;
  if (morphdic.LookUpDicVerb(s)) return s;

  int len = s.size();
  if (len > 3) {
    string suf3 = s.substr(len - 3);
    if (suf3 == "ies") return s.substr(0, len - 3) + "y";
    if (suf3 == "ing") {
      if (morphdic.LookUpDicVerb(s.substr(0, len - 3))) return s.substr(0, len - 3);
      else  return s.substr(0, len - 3)  + "e";
    }
  }
  if (len > 2) {
    string suf2 = s.substr(len - 2);
    if (suf2 == "es" || suf2 == "ed") {
      if (morphdic.LookUpDicVerb(s.substr(0, len - 2))) return s.substr(0, len - 2);
      else  return s.substr(0, len - 2)  + "e";
    }
  }
  if (len > 1) {
    string suf1 = s.substr(len - 1);
    if (suf1 == "s") return s.substr(0, len - 1);
  }
  return s;
}

static string base_form_adjective(const string & s)
{
  string ex = morphdic.BaseFormAdj(s);
  if (ex != "") return ex;

  int len = s.size();
  if (len > 3) {
    string suf3 = s.substr(len - 3);
    if (suf3 == "est") {
      if (morphdic.LookUpDicAdj(s.substr(0, len - 3) + "e")) return s.substr(0, len - 3) + "e";
      else  return s.substr(0, len - 3);
    }
  }
  if (len > 2) {
    string suf2 = s.substr(len - 2);
    if (suf2 == "er") {
      if (morphdic.LookUpDicAdj(s.substr(0, len - 2) + "e")) return s.substr(0, len - 2) + "e";
      else  return s.substr(0, len - 2);
    }
  }
  return s;
}

static string base_form_adverb(const string & s)
{
  string ex = morphdic.BaseFormAdv(s);
  if (ex != "") return ex;

  return s;
}


bool init_morphdic(const string & path)
{
  return morphdic.Init(path);
}

string base_form(const string & s, const string & pos)
{
  if (pos == "NNS") return base_form_noun(s);
  if (pos == "NNPS") return base_form_noun(s);

  if (pos == "JJR") return base_form_adjective(s);
  if (pos == "JJS") return base_form_adjective(s);

  if (pos == "RBR") return base_form_adverb(s);
  if (pos == "RBS") return base_form_adverb(s);
  
  if (pos == "VBD") return base_form_verb(s);
  if (pos == "VBG") return base_form_verb(s);
  if (pos == "VBN") return base_form_verb(s);
  if (pos == "VBP") return base_form_verb(s);
  if (pos == "VBZ") return base_form_verb(s);

  return s;
}