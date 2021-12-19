#include<iostream>
#include<fstream>
#include<string>

using namespace std;
 
class BitStream{
    public:
        void readBit(string inFile){
            ifstream f(inFile, ios::binary | ios::in);

            char c;
            f.get(c);
            for(int i = 7; i >= 0; i--) 
                    cout << ((c >> i) & 1);
        }

        void readNBit(string inFile){
            ifstream f(inFile, ios::binary | ios::in);

            char c;
            while (f.get(c))
            {
                for (int i = 7; i >= 0; i--) 
                    cout << ((c >> i) & 1);
                cout << endl;
            }
            
        }

        void writeNBit(string val, string outFile){
            ofstream o(outFile);

            for(char c : val)
            {
                for (int i = 7; i >= 0; i--) 
                    o << ((c >> i) & 1);
                o << endl;
            }
        }
};

int main(int argc, char** argv) {
  BitStream st;    
  st.writeNBit(argv[2], argv[1]);  
  return 0;
}