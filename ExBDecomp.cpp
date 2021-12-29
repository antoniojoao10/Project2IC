#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include <bitset>
#include <type_traits>
#include<climits>
#include<iostream>
#include<fstream>
#include<vector>  
#include<stdio.h>
#include<string.h>
#include<sndfile.h>
#include"Golomb.cpp"

using namespace std;

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

int main(int argc, char** argv) {
    Golomb gl;
    //ofstream ot("test.txt");
    
    ifstream ofs (argv[2], ios::in | ios::binary);

    char buffer[20];
    int cnt = 0;
    int buffcnt=0;
    vector<int> out;
    int i = 0;
    int diff = 1;
    int case0 = 0;
    int case1 = 0;
    int case2 = 0;
    int case3 = 0;

    while(1){
        ofs.read (buffer, 20); 

        if (!ofs) {
            // An error occurred!
            cnt = ofs.gcount();
            ofs.clear();
            break;
        }
        int res = gl.decoder(50000, convertToString(buffer, sizeof(buffer) / sizeof(char) ) );
        

        switch (buffcnt){
            case 0:
                case0=res;
                out.push_back(res);
                break;
            case 1:
                case1=res + case0;
                out.push_back(case1);
                break;
            case 2:
                case2 = res + 2*case1 - case0;
                out.push_back( case2 );
                break;
            case 3:
                case3 = res + 3*case2 - 3*case1 + case0;
                out.push_back( case3 );
                break;
            default:
                case0=res;
                out.push_back(res);
                buffcnt=0;
        }
        i++;
        buffcnt++;
    }

        
    //for( int i =0 ; i< out.size() ; i++) ot << out.at(i) << endl;
    
    SNDFILE* out_file;
    SF_INFO		sfinfo ;     
    sfinfo.channels = 2;
    sfinfo.samplerate = 44100;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    short outArr[out.size()];
    std::copy(out.begin(), out.end(), outArr);
    
    out_file = sf_open(argv[1], SFM_WRITE, &sfinfo);
    sfinfo.frames=out.size();
    sf_write_short(out_file, outArr , sfinfo.channels * sfinfo.frames);
    sf_close(out_file);
    
    return 0;
}
