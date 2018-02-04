#include <boost/foreach.hpp>
#include <array>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>

class Schedule {
  public:
    Schedule(double initial_value, double step_size) {
      this->current_value = this->initial_value = initial_value;
      this->step_size = step_size;
      this->steps = initial_value / step_size;
      this->step_value = initial_value / steps;
    }
    operator double() {
      double value = current_value;
      if(current_value >= step_value)
        current_value -= step_value;
      return value;
    };
    friend std::ostream& operator<<(std::ostream& os, const Schedule& sched) {
      os << '[' << sched.initial_value << ',' << sched.current_value << ',' << sched.step_value << ',' << sched.step_size << ',' << sched.steps << ']';
      return os;
    };
  private:
    double initial_value;
    double current_value;
    double step_value;
    double step_size;
    double steps;
};

class Coordinate {
  public:
    Coordinate() { x = 0.0 ; y = 0.0; }
    Coordinate(double x, double y) { this->x = x ; this->y = y; }
    double x;
    double y;
    friend std::ostream& operator<<(std::ostream& os, const Coordinate& coord) {
      os << '[' << coord.x << ',' << coord.y << ']';
    };
    Coordinate& operator=(const Coordinate& other) {
      this->x = other.x;
      this->y = other.y;
      return *this;
    }
};

class Map {
  public:
    double total_distance(std::array<Coordinate, 3> airports);

  private:
    std::array<Coordinate, 20> cities{{
      Coordinate(43,57),
      Coordinate(65,59),
      Coordinate(5,41),
      Coordinate(9,52),
      Coordinate(9,2),
      Coordinate(74,81),
      Coordinate(65,73),
      Coordinate(18,90),
      Coordinate(84,78),
      Coordinate(24,28),
      Coordinate(44,83),
      Coordinate(96,94),
      Coordinate(89,30),
      Coordinate(84,8),
      Coordinate(11,86),
      Coordinate(61,100),
      Coordinate(6,88),
      Coordinate(71,74),
      Coordinate(37,19),
      Coordinate(7,4)
   }};
};

double Map::total_distance(std::array<Coordinate, 3> airports) {
  double total_distance = 0.0;
  BOOST_FOREACH(Coordinate city, cities) {
    double minimum_distance = std::numeric_limits<double>::max();
    BOOST_FOREACH(Coordinate airport, airports) {
      double single_distance = sqrt(pow(city.x-airport.x,2)+pow(city.y-airport.y,2));
      if(minimum_distance > single_distance) minimum_distance = single_distance;
    };
    total_distance += minimum_distance;
  };
  return total_distance;
};

std::array<Coordinate, 3> random_successor(std::array<Coordinate, 3> airports) {
  while(true) {
    int r = std::rand() % 27;
    int y = r % 3 - 1;
    int x = ((r / 3) % 3) - 1;
    int a = (r / 9);
    std::array<Coordinate, 3> successor = airports;
    successor[a].x += x;
    successor[a].y += y;
    if(successor[a].x >= 0 && successor[a].y >= 0 && successor[a].x < 100 && successor[a].y < 100)
      return successor;
  };
}

std::string sarray(std::array<Coordinate, 3> array) {
  std::ostringstream ss;
  ss << '[';
  for(int x = 0 ; x < 3 ; x++)
    ss << array[x];
  ss << ']';
  return ss.str();
};

void simulated_annealing() {
  class Map m;
  class Schedule schedule(40.0, 40.0/10000000.0); // Total guess! We'll go with an initial value of 1MM and a step size of 1
  std::array<Coordinate, 3> current{{Coordinate(100,100),Coordinate(100,100),Coordinate(100,100)}};
  std::array<Coordinate, 3> best = current;
  double T;
  while((T = schedule) >= 1) {
    if(m.total_distance(current) < m.total_distance(best)) {
      best = current;
      std::cout << "Saving new best: " << sarray(best) << ", distance=" << m.total_distance(best) << std::endl << std::flush;
    };
    std::array<Coordinate, 3> next = random_successor(current);
    double td_next = m.total_distance(next);
    double td_current = m.total_distance(current);
    double delta_e = td_current - td_next;
#if 1
    std::cout << "Current: " << sarray(current) << ",Next: " << sarray(next) << 
                 ",delta: " << delta_e <<
                 ",current dist: " << m.total_distance(current) <<
                 ",next dist: " << m.total_distance(next) <<
                 std::endl << std::flush;
    std::cout << "delta_e:" << delta_e << ", T:" << T << ",exp: " << exp(delta_e/T);// << std::endl << std::flush;
#endif
    if(delta_e > 0) {
      current = next;
#if 1
      std::cout << " keeping best node" << std::endl << std::flush;
#endif
    } else if(std::exp(delta_e/T) >= (double)std::rand() / (double)std::numeric_limits<int>::max()) {
      current = next;
#if 1
      std::cout << " TRYING worse node" << std::endl << std::flush;
    } else {
      std::cout << " not trying worse node" << std::endl << std::flush;
#endif
    };
  };
  std::cout << "The closest we got is :" << std::endl;
  BOOST_FOREACH(Coordinate ap, current) {
    std::cout << ap;
  };
  std::cout << ", distance=" << m.total_distance(current);
  std::cout << std::endl;
}

int main() {
//  Map m;
//  std::array<Coordinate, 3> c1{{Coordinate(11,86),Coordinate(23,26),Coordinate(71,74)}};
//  std::array<Coordinate, 3> c2{{Coordinate(23,24),Coordinate(69,74),Coordinate(12,87)}};
//  std::cout << "c1=" << m.total_distance(c1) << std::endl;
//  std::cout << "c2=" << m.total_distance(c2) << std::endl;
//  exit(0);
  std::srand(std::time(NULL));
  simulated_annealing();
}
