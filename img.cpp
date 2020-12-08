#include <iostream>
#include <fstream>

using namespace std;

int main() {
    ofstream outfile;
    outfile.open("/tmp/position.txt", ios::out);
    outfile<<"{\"x\":100,\"y\":200,:\"width\":300,\"hight\":400}"<<endl;
    cout<<"write finish"<<endl;
    outfile.close();
    return 0;
}
