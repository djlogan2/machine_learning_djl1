#include <boost/foreach.hpp>
#include <algorithm>
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
    static Coordinate random() {
      return Coordinate(std::rand() % 100, std::rand() %100);
    };
    double x;
    double y;
    friend std::ostream& operator<<(std::ostream& os, const Coordinate& coord) {
      os << '[' << coord.x << ',' << coord.y << ']';
    };
    Coordinate operator-() {
      return Coordinate(-x, -y);
    }
    Coordinate operator-(const Coordinate& other) {
      return Coordinate(x-other.x, y-other.y);
    }
    Coordinate &operator*=(const double other) {
      x *= other;
      y *= other;
      return *this;
    }
    Coordinate operator*(const Coordinate& other) {
      return Coordinate(x*other.x, y*other.y);
    }
    Coordinate operator*(const double value) {
      return Coordinate(x*value, y*value);
    }
    Coordinate operator/(const Coordinate& other) {
      return Coordinate(x/other.x, y/other.y);
    }
    Coordinate operator/(const double value) {
      return Coordinate(x/value, y/value);
    }
    Coordinate &operator/=(const double value) {
      x /= value;
      y /= value;
      return *this;
    }
    bool operator<=(const Coordinate& other) {
      return (x <= other.x && y <= other.y);
    };
    bool operator>=(const Coordinate& other) {
      return (x >= other.x && y >= other.y);
    };
    bool operator<=(const double value) {
      return (x <= value && y <= value);
    };
    bool operator>=(const double value) {
      return (x >= value && y >= value);
    };
    bool operator<(const double value) {
      return (x < value || y < value);
    };
    bool operator>(const double value) {
      return (x > value || y > value);
    };
    Coordinate &operator-=(const Coordinate& other) {
      this->x -= other.x;
      this->y -= other.y;
      return *this;
    }
    Coordinate &operator+=(const Coordinate& other) {
      this->x += other.x;
      this->y += other.y;
      return *this;
    }
    Coordinate& operator=(const Coordinate& other) {
      this->x = other.x;
      this->y = other.y;
      return *this;
    }
};

template<class T, int N>
std::string sarray(std::array<T, N> array) {
  std::ostringstream ss;
  ss << '[';
  for(int x = 0 ; x < array.size() ; x++)
    ss << array[x] << ',';
  ss << ']';
  return ss.str();
};

class Map {
  public:
    double total_distance(std::array<Coordinate, 3> airports);
    Coordinate sum();
    std::array<Coordinate, 3> distance_to_zero(std::array<Coordinate, 3> airports, bool orig=false);
    int closest_airport(Coordinate city, std::array<Coordinate, 3> airports);
    std::array<int, 20> closest_airports(std::array<Coordinate, 3> airports);
  private:
    Coordinate _sum;
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

class {
  Map m;
  public:
    bool operator() (std::array<Coordinate, 3>*s1, std::array<Coordinate, 3>*s2)
    {
      if((*s1)[0] == 0) return false;
      if((*s2)[0] == 0) return true;
      return m.total_distance(*s1) <= m.total_distance(*s2);
    }
} possiblessort;


Coordinate Map::sum() {
  if(_sum.x != 0) return _sum;
  BOOST_FOREACH(Coordinate city, cities) {
    _sum += city;
  };
  return _sum;
};

std::array<Coordinate, 3> Map::distance_to_zero(std::array<Coordinate, 3> airports, bool orig) {
  std::array<Coordinate, 3> sum;
  std::array<Coordinate, 3> grad;
  std::array<int, 3> city_count{{0,0,0}};
  //
  // Sum the X and Y values for the closest airport to each city
  //
  BOOST_FOREACH(Coordinate city, cities) {
    int which = closest_airport(city, airports);
    city_count[which]++;
    sum[which] += city;
  };
  //
  // Now get the zero of the gradient for each airport
  //
  for(int i = 0 ; i < 3 ; i++)
    if(city_count[i])
      grad[i] = airports[i]-(sum[i]/city_count[i]);

  if(orig) return grad;

  double previous_distance = std::numeric_limits<double>::max();
  double current_distance = total_distance(airports);
  double previous_step = 1/std::pow(2,8);
  double current_step = previous_step;
  std::array<Coordinate, 3> test;
  bool outofbounds;

  do {
    outofbounds = false;
    previous_distance = current_distance; 
    previous_step = current_step;
    current_step *= 2;
    for(int i = 0 ; i < 3 ; i++) {
      test[i] = airports[i] - (grad[i]*current_step);
      if(test[i] < 0 || test[i] > 100)
        outofbounds = true;
    };
    if(!outofbounds)
      current_distance = total_distance(test);
    else
      current_distance = std::numeric_limits<double>::max();
  } while(current_distance < previous_distance);

  for(int i = 0 ; i < 3 ; i++) grad[i] *= previous_step;
  return grad;
};

std::array<int, 20> Map::closest_airports(std::array<Coordinate, 3> airports) {
  std::array<int, 20> city_count={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
  for(int x = 0 ; x < 20 ; x++) {
    city_count[x] = closest_airport(cities[x], airports);
  };
  return city_count;
}

int Map::closest_airport(Coordinate city, std::array<Coordinate, 3> airports) {
    double minimum_distance = std::numeric_limits<double>::max();
    int which = std::numeric_limits<int>::max();
    for(int i = 0 ; i < 3 ; i++) {
      double single_distance = sqrt(pow(city.x-airports[i].x,2)+pow(city.y-airports[i].y,2));
      if(minimum_distance > single_distance) {
        minimum_distance = single_distance;
        which = i;
      };
  };
  return which;
}

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
    if(successor[a]>= 0 && successor[a] <= 100)
      return successor;
  };
}

std::array<Coordinate, 3> gradient_successor(std::array<Coordinate, 3> airports) {
  Map m;
  std::array<std::array<Coordinate, 3>, 19> possibles;
  std::array<Coordinate, 3> dtz = m.distance_to_zero(airports);
  int psize = 0;
  for(int a = 0 ; a < 3 ; a++) {
    for(int x = -1 ; x <= 1 ; x++) {
      for(int y = -1 ; y <= 1 ; y++) {
        possibles[psize][a].x = airports[a].x + x;
        possibles[psize][a].y = airports[a].x + y;
        psize++;
      };
    };
  };

  possibles[psize] = airports; // Copy over originals in case the gradient if statements only replace part of them
  //
  // Now use any gradient minimums we have found. If we have found any, use the ones we found, leaving
  // any other airports original values if they don't have a valid gradient value. We presume that if
  // They don't have a valid gradient value, then they are already at their local minimum.
  //
  bool gradients = false;
  bool outofbounds = false;
  for(int i = 0 ; i < 3 ; i++) {
    if(std::abs(dtz[i].x) >= 1.0 || std::abs(dtz[i].y) >= 1.0) {
      Coordinate test = possibles[0][i] - dtz[i];
      if(test < 0 || test > 100) {
        outofbounds = true;
      } else {
        possibles[psize][i] = test;
        gradients = true;
      };
    };
  };

  if(gradients && !outofbounds) {
    int g = psize++;
    for(int a = 0 ; a < 3 ; a++) {
      for(int x = -1 ; x <= 1 ; x++) {
        for(int y = -1 ; y <= 1 ; y++) {
          possibles[psize][a].x = possibles[g][a].x + x;
          possibles[psize][a].y = possibles[g][a].x + y;
          psize++;
        };
      };
    };
  };
  std::sort(possibles.begin(), possibles.end(), possiblessort);
  return possibles[0];
}

void simulated_annealing() {
  class Map m;
  class Schedule schedule(40.0, 40.0/10000000.0); // Total guess! We'll go with an initial value of 1MM and a step size of 1
  std::array<Coordinate, 3> current{{Coordinate(100,100),Coordinate(100,100),Coordinate(100,100)}};
  std::array<Coordinate, 3> best = current;
  double T;
  while((T = schedule) >= 1) {
    if(m.total_distance(current) < m.total_distance(best)) {
      best = current;
      std::cout << "Saving new best: " << sarray<Coordinate, 3>(best) << ", distance=" << m.total_distance(best) << std::endl << std::flush;
    };

    std::array<Coordinate, 3> next = gradient_successor(current);
    double td_next = m.total_distance(next);
    double td_current = m.total_distance(current);
    double delta_e = td_current - td_next;
#if 1
    std::cout << "Current: " << sarray<Coordinate, 3>(current) << ",Next: " << sarray<Coordinate, 3>(next) << 
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
    } else { //if(std::exp(delta_e/T) >= (double)std::rand() / (double)std::numeric_limits<int>::max()) {
      current = next;
#if 1
      std::cout << " TRYING worse node" << std::endl << std::flush;
//    } else {
//      std::cout << " not trying worse node" << std::endl << std::flush;
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
  Map m;
//[[11.4943,87.4059][68.7248,73.5887][24.6982,21.5338]]
//$8 = {x = 10.333333333333334, y = 84}
//$9 = {x = 72.799999999999997, y = 75.099999999999994}
//$10 = {x = 21, y = 30}
//Saving new best: [[10,86][69,74][19,22]], distance=418.728
  std::array<Coordinate, 3> best1 {{Coordinate(11.4943,87.4059),Coordinate(68.7248,73.5887),Coordinate(24.6982,21.5338)}};
  std::array<Coordinate, 3> best2 {{Coordinate(11,86),Coordinate(71,74),Coordinate(23,26)}};
  std::array<Coordinate, 3> best3 {{Coordinate(10,86),Coordinate(69,74),Coordinate(19,22)}};
  double dist1 = m.total_distance(best1);
  double dist2 = m.total_distance(best2);
  double dist3 = m.total_distance(best3);
  std::array<Coordinate, 3> grad1 = m.distance_to_zero(best1, true);
  std::array<Coordinate, 3> grad2 = m.distance_to_zero(best2, true);
  std::array<Coordinate, 3> grad3 = m.distance_to_zero(best3, true);
  std::array<Coordinate, 3> next1;
  std::array<Coordinate, 3> next2;
  std::array<Coordinate, 3> next3;
  for(int i = 0 ; i < 3 ; i++) next1[i] = best1[i] - grad1[i];
  for(int i = 0 ; i < 3 ; i++) next2[i] = best2[i] - grad2[i];
  for(int i = 0 ; i < 3 ; i++) next3[i] = best3[i] - grad3[i];
  double dist4 = m.total_distance(next1);
  double dist5 = m.total_distance(next2);
  double dist6 = m.total_distance(next3);
//  std::cout << "c1=" << m.total_distance(c1) << std::endl;
//  std::cout << "c2=" << m.total_distance(c2) << std::endl;
//  exit(0);
//  std::array<Coordinate, 6> minimum = m.minimum(std::array<Coordinate, 3>{{Coordinate(22,26), Coordinate(68,73), Coordinate(10,86)}});
//  exit(0);
  std::srand(std::time(NULL));
  simulated_annealing();
}
