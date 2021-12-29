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
#include <map>
#include <iterator>
#include "Golomb.cpp"

using namespace std;

int main(int argc, char** argv) {
    Golomb gl;
    SNDFILE* read_file;
    SNDFILE* out_file;

    //ofstream ot ("test1.txt");
    
    //ofstream ofs (argv[2]);
    ofstream ofs (argv[2], ios::out | ios::binary);
    
 
    SF_INFO		sfinfo ;     
    read_file = sf_open(argv[1], SFM_READ, &sfinfo);
    short *tmpIn = new short[sfinfo.channels * sfinfo.frames];
    sf_read_short(read_file, tmpIn, sfinfo.channels * sfinfo.frames);
    int channels = sfinfo.channels;
    int frame = sfinfo.frames;
    sf_close(read_file);     
    vector<int> re;
    map<int,int> histMap;
    int cnt =0; 
    int p = 0;
    for(int i  =0; i < frame*channels ; i++){
        switch (cnt){
            case 0:
                break;
            case 1:
                p = tmpIn[i-1];
                break;
            case 2:
                p = 2*tmpIn[i-1] - tmpIn[i-2];
                break;
            case 3:
                p = 3*tmpIn[i-1] - 3*tmpIn[i-2] + tmpIn[i-3];
                break;
            default:
                p=0;
                cnt=0;
        }
        cnt++;

        int resi = tmpIn[i] - p;

        
        int tmp = resi >> 12;
        resi = tmp << 12;
        int tp = resi;
        if( tp - resi == 0)  resi = tp;
        else resi = tp/(tp-resi);

        re.push_back(resi);

        //histogram
        histMap[resi] ++;


        //ot << re.at(i) << endl;

        string res = gl.encoder( 50000 , re.at(i));

        //ofs << res << endl;

        int sz = gl.encoder( 50000 , re.at(i)).length();

        // declaring character array
        char arr[sz + 1];
    
        // copying the contents of the
        // string to char array
        strcpy(arr, gl.encoder( 50000 , re.at(i)).c_str());
        ofs.write(arr, 20); //
        
    } 

    ofstream hist("hist.txt");

    map<int, int>::iterator itr;
    for (itr = histMap.begin(); itr != histMap.end(); ++itr) {
        hist << itr->first << "\t ----> \t" << itr->second << endl;
    }
    hist.close();
    return 0;
}