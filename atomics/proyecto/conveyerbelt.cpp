#include "conveyerbelt.h"

void conveyerbelt::init(double t,...) {
  va_list parameters;
  va_start(parameters,t);

  velocity = va_arg(parameters, double);
  length = va_arg(parameters, double);

  sigma = numeric_limits<double>::max();
}

double conveyerbelt::ta(double t) {
  return sigma;
}

void conveyerbelt::dint(double t) {
  PcBoxes = updateAllDistances(PcBoxes, e, velocity);
  PlayerBoxes = updateAllDistances(PlayerBoxes, e, velocity);
  pair<double,double> PcBox = PcBoxes.front();
  pair<double,double> PlayerBox = PlayerBoxes.front();

  if (PlayerBoxes.empty()) {
    PcBoxes.pop_front();
  } else if (PcBoxes.empty()) {
    PlayerBoxes.pop_front();
  } else {
    double PcCollisionPower = collisionPower(PcBox);
    double PlayerCollisionPower = collisionPower(PlayerBox);

    PcBoxes.pop_front();
    PlayerBoxes.pop_front();
    if (!isEqual(PlayerCollisionPower, PcCollisionPower)) {
      if (PlayerCollisionPower < PcCollisionPower) {
        pair<double,double> box (updatedWeight(PcBox, PlayerBox), PcBox.second);
        PcBoxes.push_front(box);
      }
      if (PlayerCollisionPower > PcCollisionPower) {
        pair<double,double> box (updatedWeight(PlayerBox, PcBox), PlayerBox.second);
        PlayerBoxes.push_front(box);
      }
    }
  }
  
  sigma = nextEvent(PcBoxes, PlayerBoxes, length, velocity);
}

void conveyerbelt::dext(Event x, double t) {
  in = *(double*) x.value;
  pair<double,double> box (in, 0.0);

  PcBoxes = updateAllDistances(PcBoxes, e, velocity);
  PlayerBoxes = updateAllDistances(PlayerBoxes, e, velocity);
  if (x.port == PC) {
    PcBoxes.push_back(box);
  }
  if (x.port == Player) {
    PlayerBoxes.push_back(box);
  }

  sigma = nextEvent(PcBoxes, PlayerBoxes, length, velocity);

  /*
    const char* who = x.port == 0 ? "PC" : "Player";
    printLog("Entro %s Pasaron %f s\n", who, e);
    printLog("Sigma %f\n", sigma);
    printLog("PC %f %f %d \n", PcBoxes.front().second, PcBoxes.back().second, PcBoxes.size());
    printLog("Player %f %f %d \n", PlayerBoxes.front().second, PlayerBoxes.back().second, PlayerBoxes.size());
    printLog("\n");
  */    
}

Event conveyerbelt::lambda(double t) {
  PcDistance = updatedDistance(PcBoxes.front().second, sigma, velocity);
  PlayerDistance = updatedDistance(PlayerBoxes.front().second, sigma, velocity);
  pair<double,double> PcBox (PcBoxes.front().first, PcDistance);
  pair<double,double> PlayerBox (PlayerBoxes.front().first, PlayerDistance);

  if (PlayerBoxes.empty()) {
    // printLog("LlegoPc\n");
    out = make_tuple(PC_ARRIVAL, PcBox.first, length);
    return Event(&out, ARRIVALS_PORT);
  } else if (PcBoxes.empty()) {
    // printLog("LlegoPlayer\n");
    out = make_tuple(PLAYER_ARRIVAL, PlayerBox.first, length);
    return Event(&out, ARRIVALS_PORT);
  } else {
    double PcCollisionPower = collisionPower(PcBox);
    double PlayerCollisionPower = collisionPower(PlayerBox);

    if (isEqual(PlayerCollisionPower, PcCollisionPower)) {
      // printLog("ColisionEmpate\n");
      out = make_tuple(COLLISION_DRAW, 0, PlayerDistance);
      return Event(&out, COLLISIONS_PORT);
    } else if (PlayerCollisionPower < PcCollisionPower) {
      // printLog("ColisionGanoPc\n");
      out = make_tuple(COLLISION_PC_WINS, updatedWeight(PcBox, PlayerBox), PcDistance);
      return Event(&out, COLLISIONS_PORT);
    } else if (PlayerCollisionPower > PcCollisionPower) {
      // printLog("ColisionGanoPlayer\n");
      out = make_tuple(COLLISION_PLAYER_WINS, updatedWeight(PlayerBox, PcBox), PlayerDistance);
      return Event(&out, COLLISIONS_PORT);
    }
  }

  return Event();
}

void conveyerbelt::exit() {
  // printLog("Fin Cinta\n");
}
