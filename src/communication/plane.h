#ifndef __COMM_PLANE__
#define __COMM_PLANE__

int comm_check_destinations(struct PlaneThread* plane, int* destinations, size_t* len);

int comm_set_destination(struct PlaneThread* plane, int target);

#endif
