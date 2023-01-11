//TODO: Merge files into one. We're not experienced with multi-file programming. May reserve a header file for future use.

#include<curl/curl.h>
#include<iostream>
#include<string>
#include<filesystem>
using namespace std;
using namespace std::filesystem;
CURL* curl_handle;
string username;
int main(int argc, const char** argv)
{
    /*
     * 1. Fetch information from websites (CAO, CKO)
     * 2. Find the links in fetched indexes
     * 3. Download morgue files
     * 4. Grab the informatino from morgues
     * 5. Output stats, and give some comments (implememt later)
     */
    return 0;
}