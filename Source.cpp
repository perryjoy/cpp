#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>

using namespace std;
 

typedef class text
{
  ifstream input_source;
  ofstream output_dest;
  vector<string> data;
  string buffer;
  size_t str_len;

  //~text(void) 
  //{
  //  input_source.close();
  //}
  //{
  //  if (input_source.is_open)
  //    input_source.close;
  //  buffer.~basic_string();
  //  data.clear;
  //}

  void typewords(size_t &currLen)
  {
    if (data.size() != 0)
    {
      size_t spaceDifference = str_len - currLen - data.size() + 1;
      for (size_t i = 0; i < spaceDifference / 2; i++)
        (output_dest.is_open() ? output_dest : cout) << " ";
 /*     for (auto &t : data)
        (output_dest.is_open() ? output_dest : cout) << ((t != data.front()) ? " " : "") << t ;*/
      for (size_t i = 0; i < data.size(); i++)
        (output_dest.is_open() ? output_dest : cout) << ((i != 0) ? " " : "") << data[i];
      for (size_t i = 0; i + spaceDifference / 2 < spaceDifference; i++)
        (output_dest.is_open() ? output_dest : cout) << " ";

      (output_dest.is_open() ? output_dest : cout) << endl;
      currLen = 0;
      data.clear();
    }
    else
      assert(currLen == 0);
  }

  void getstr(void)
  {
 // try // Ive tried but failed horribly
   // size_t currLen;
    string word;
    size_t currLen = 0;
    size_t wBegin, wEnd;
    getline(input_source, buffer);
   
    wBegin = find_if_not(buffer.begin(), buffer.end(), isspace) - buffer.begin();
    wEnd = buffer.find(' ', wBegin);
    while (wBegin != string::npos)
    {
      if (wEnd == string::npos)
        word = buffer.substr(wBegin);
      else
        word = buffer.substr(wBegin, wEnd - wBegin);

      incomplete_word:
      if (currLen + word.size() + data.size() >= str_len)
        if (word.size() <= str_len)  // jumping on next string
        {
          typewords(currLen);
          data.push_back(word);
          currLen += word.size();
          word.clear();
          goto incomplete_word;
        }
        else
        {
          data.push_back(word.substr(0, str_len - currLen - data.size())); // cutting word if its too big
          word.erase(0, data.back().size());
          currLen = str_len - data.size() + 1;
          typewords(currLen);
          goto incomplete_word;
        }
      else
      {
        if (word.size() > 0)
        {
          data.push_back(word);
          currLen += word.size();
        }
      }
      
      if (wEnd != string::npos)
        wBegin = find_if_not(buffer.begin() + wEnd, buffer.end(), isspace) - buffer.begin();
      else
      {
        typewords(currLen);
        break;
      }
      wEnd = buffer.find(' ', wBegin);
    }
  }

 public: 

  text(size_t len = 10, string name = "", string out = "") //(size_t len = 10, string name = string("")) - doesnt work and i dont know why
  {
    if (name.size() == 0)
      input_source = ifstream(stdin);
    else
      input_source.open(name, fstream::in);

    if (out.size() == 0)
      output_dest = ofstream(stdout);
    else
      output_dest.open(out, fstream::out);

    str_len = len;
  }

  void gettext(void)
  {
    while (!input_source.eof())
      getstr();
  }


} text;



int main(int argc, char** argv)
{
  string inp = (argc == 1 ? string("") : string(argv[1])) , outp = (argc <= 2 ? string("") : string(argv[2]));

  //text p = text(12, "test.txt", "res.txt");

  text p = text(12, inp, outp);

  p.gettext();

  //for (auto &q : res)
  //  cout << q << endl;


  return 0;
}