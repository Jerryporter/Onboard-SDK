#include <iostream>
#include <fstream>
using namespace std;
int main() {
    ifstream inputpositon;
    system("bash /home/chy/Desktop/setposition.sh");
    inputpositon.open("/tmp/position.txt");
    string position;
    inputpositon>>position;
    cout<<position;
    return 0;
}
