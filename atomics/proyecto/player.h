//FLAGS:-std=c++11
//CPP:proyecto/player.cpp
//CPP:proyecto/numberGenerator.cpp
//CPP:proyecto/strategies.cpp
#if !defined player_h
#define player_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "limits"
#include "strategies.h"
#include <list>

class player: public Simulator { 
	std::list<double> weights;
	std::list<double> interarrivals;
	int strategy;
	int quantity;
	char* name;

	double out;
	double sigma;

	public:
		player(const char *n): Simulator(n) {};
		void init(double, ...);
		double ta(double t);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
		void exit();
};
#endif
