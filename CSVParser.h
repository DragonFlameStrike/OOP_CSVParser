//
// Created by 1 on 28.11.2021.
//

#ifndef CSVPARSER_CSVPARSER_H
#define CSVPARSER_CSVPARSER_H
#include <fstream>
#include "TupleUtils.h"

using namespace std;

template<class ... Args>
class CSVParser
{
private:
    ifstream &input;
    size_t offset;
    int file_lenght = -1;
    char column_delimiter = ',';

    int get_length()

    {
        if(file_lenght == -1)
        {
            //reset
            input.clear();
            input.seekg(0, ios::beg);

            string line;
            for(file_lenght = 0; getline(input, line); file_lenght++);
            //reset
            input.clear();
            input.seekg(0, ios::beg);
        }
        return file_lenght;
    }

    class CSVIterator
    {
    private:
        string buffer;
        ifstream &input;
        size_t index;
        CSVParser<Args...> &parent;
        bool last = false;
        friend class CSVParser;

    public:
        CSVIterator(ifstream& ifs, size_t index, CSVParser<Args...>& parent) : index(index), parent(parent), input(ifs)
        {
            for (int i = 0; i < index - 1; i++, getline(input, buffer));

            getline(input, buffer);
        }

        CSVIterator operator++()
        {
            if(index < parent.file_lenght)
            {
                index++;
                //reset
                input.clear();
                input.seekg(0, ios::beg);
                for (int i = 0; i < index - 1; ++i, getline(input, buffer));

                getline(input, buffer);
            } else
            {
                buffer = "";
                last = true;
            }

            return *this;
        }

        bool operator==(const CSVIterator &other) const
        {
            return this->last == other.last && this->index == other.index && this->buffer == other.buffer;
        }

        bool operator!=(const CSVIterator &other)
        {
            return !(*this == other);
        }

        tuple<Args...> operator*()
        {
            return parent.parse_line(buffer, index);
        }
    };

public:
    explicit CSVParser(ifstream &ifs, size_t offset) : input(ifs), offset(offset)
    {
        if(!ifs.is_open())
            throw std::invalid_argument("Can't open file");
        if(offset >= get_length())
            throw logic_error("Bad file offset! offset >= file");
        if(offset < 0)
            throw logic_error("Bad file offset! offset < 0");
    }

    CSVIterator begin()
    {
        CSVIterator a(input, offset + 1, *this);
        return a;
    }

    CSVIterator end()
    {
        CSVIterator a(input, get_length(), *this);
        a.last = true;
        return a;
    }

    vector<string> read_string(string& line)
    {
        vector<string> fields{ "" };
        size_t fcounter = 0;
        for (char c : line)
        {
            if (c == column_delimiter)
            {
                fields.emplace_back("");
                fcounter++;
            }
            else
            {
                fields[fcounter].push_back(c);
            }
        }
        return fields;
    }

    tuple<Args...> parse_line(string &line, int number)
    {
        size_t size = sizeof...(Args);

        if(line.empty())
            throw invalid_argument("Line " + to_string(number) + " is empty!");

        tuple<Args...> table_str;
        vector<string> fields = read_string(line);

        if(fields.size() != size)
            throw invalid_argument("Wrong number of fields in line " + to_string(number) + "!");

        auto a = fields.begin();
        try
        {
            parser_utils::parse(table_str, a);
        }catch(exception&e)
        {
            throw invalid_argument("Line " + to_string(number) + " contains bad types!");
        }

        return table_str;
    }
};

#endif //CSVPARSER_CSVPARSER_H
