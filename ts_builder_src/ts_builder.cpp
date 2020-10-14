#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <iostream>
using namespace std;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
    vector<string> inputs, outputs;

    bool isInput = true;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-i")) {
            isInput = true;
        } else if (!strcmp(argv[i], "-o")) {
            isInput = false;
        } else {
            (isInput ? inputs : outputs).emplace_back(argv[i]);
        }
    }

    assert(inputs.size() == outputs.size());

    int len = inputs.size();
    for (int i = 0; i < len; ++i) {
        auto &input = inputs[i];
        auto &output = outputs[i];

        printf("%s -> %s\n", input.c_str(), output.c_str());

        auto lang = fs::path(input).stem().string();
        auto list = fs::path(".") / (lang + ".lst");
        if (fs::exists(list)) {
            printf("processing lang: %s\n", lang.c_str());

            map<string, string> translations;

            ifstream infs;
            infs.open(list);
            ofstream outfs;

            for (std::string line; std::getline(infs, line); ) {
                if (line.length() == 0) continue;

                auto delimiter = line.find(' ');

                if (delimiter == 0) continue;

                if (delimiter == string::npos) {
                    translations[line] = "";
                } else {
                    translations[line.substr(0, delimiter)] = line.substr(delimiter + 1);
                }
            }

            infs.close();
            infs.open(input);
            outfs.open(output, ios_base::trunc);

            regex regex_source("\\s*<source>(\\w+)</source>\\s*");
            regex regex_translation("\\s*<translation[^>]*>[^<]*</translation>\\s*");

            smatch match;
            for (std::string line; std::getline(infs, line); ) {
                if (regex_match(line, match, regex_translation)) {
                    continue;
                }

                if (regex_match(line, match, regex_source)) {
                    auto key = match[1].str();
                    auto replacement = translations.find(key) != translations.end() ? translations[key] : key;

                    outfs << line << endl << "<translation>" << replacement << "</translation>" << endl;
                } else {
                    outfs << line << endl;
                }
            }

            infs.close();
            outfs.close();

            // fs::copy(input, output, fs::copy_options::overwrite_existing);
        } else {
            printf("skip unknown lang: %s\n", lang.c_str());
            fs::copy(input, output, fs::copy_options::overwrite_existing);
        }
    }

    return 0;
}
