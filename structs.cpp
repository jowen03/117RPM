#include <string>
using namespace std;

#include "structs.idl"

Person findPerson(ThreePeople tp) {
    return tp.p3;
}

int area(rectangle r) {
    return r.x * r.y;
}