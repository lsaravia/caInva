#include <iostream>
#include <time.h>

int main(int argc, char * argv[])
{
	cout << argv[1] << endl;   // Title
	cout << argv[2] << endl;   // Control File
	cout << argv[3] << endl;   // From Data file
	cout << "(2f7.2)" << endl;   //
	cout << argv[4] << endl;   // TO Data file
	cout << "(2f7.2)" << endl;   //
	cout << argv[5] << endl;   // Full report file
	cout << argv[6] << endl;   // Summary report file
	cout << "    1  200     0.025" << endl;
	cout << "    1  ";
	int t=time(NULL);
    cout << t << endl;

}
