#include "conveyerbelt.h"

void conveyerbelt::init(double t,...) {
  va_list parameters;
  va_start(parameters,t);

  velocity = va_arg(parameters, double);
  length = va_arg(parameters, double);
  printLog("parametros %f %f\n", velocity, length);

  sigma = std::numeric_limits<double>::max();
}

double conveyerbelt::ta(double t) {
  return sigma;
}

void conveyerbelt::dint(double t) {
  PcBoxes = updateAllDistances(PcBoxes, e, velocity);
  PlayerBoxes = updateAllDistances(PlayerBoxes, e, velocity);
  std::pair<double,double> PcBox = PcBoxes.front();
  std::pair<double,double> PlayerBox = PlayerBoxes.front();

  if (isEqual(PcBoxes.front().second, length)) {
    PcBoxes.pop_front();
  } else if (isEqual(PlayerBoxes.front().second, length)) {
    PlayerBoxes.pop_front();
  } else {
    double PcCollisionPower = collisionPower(PcBox);
    double PlayerCollisionPower = collisionPower(PlayerBox);

    PcBoxes.pop_front();
    PlayerBoxes.pop_front();
    if (PlayerCollisionPower < PcCollisionPower) {
      std::pair<double,double> box (updatedWeight(PcBox, PlayerBox), PcBox.second);
      PcBoxes.push_front(box);
    }
    if (PlayerCollisionPower > PcCollisionPower) {
      std::pair<double,double> box (updatedWeight(PlayerBox, PcBox), PlayerBox.second);
      PlayerBoxes.push_front(box);
    }
  }
  
  sigma = nextEvent(PcBoxes, PlayerBoxes, length, velocity);
}

void conveyerbelt::dext(Event x, double t) {
  in = *(double*) x.value;
  std::pair<double,double> box (in, 0.0);

  PcBoxes = updateAllDistances(PcBoxes, e, velocity);
  PlayerBoxes = updateAllDistances(PlayerBoxes, e, velocity);
  if (x.port == PC) {
    PcBoxes.push_back(box);
  }
  if (x.port == Player) {
    PlayerBoxes.push_back(box);
  }

  sigma = nextEvent(PcBoxes, PlayerBoxes, length, velocity);

  const char* who = x.port == 0 ? "PC" : "Player";
  printLog("Entro %s Pasaron %f s\n", who, e);
  printLog("Sigma %f\n", sigma);
  printLog("PC %f %f %d \n", PcBoxes.front().second, PcBoxes.back().second, PcBoxes.size());
  printLog("Player %f %f %d \n", PlayerBoxes.front().second, PlayerBoxes.back().second, PlayerBoxes.size());
  printLog("\n");
    
}

Event conveyerbelt::lambda(double t) {
  printLog("Llegue a 0\n");
  return Event();
}

void conveyerbelt::exit() {
  printLog("Fin Cinta\n");
}
