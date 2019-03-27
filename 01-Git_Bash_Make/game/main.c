#include <iostream>
#include <stdlib.h>

using namespace std ;

int main(void)
{
	int a, b;

	cout < < "Please insert number beetwen 0 and 9\n";
	cin >> a;
	b = random()%10;
	if (a == b) {
	    cout << "you won\n";

} else
cout << "you lose\n";
	return 0;
}
