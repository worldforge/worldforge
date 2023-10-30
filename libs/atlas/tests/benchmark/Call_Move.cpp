#include "timer.h"
#include <iostream>

class NPC
{
public:
  NPC() {x=y=z = vx=vy=vz = 0.0;}
  void move(double new_vx, double new_vy, double new_vz,
            double &res_x, double &res_y, double &res_z,
            double &res_vx, double &res_vy, double &res_vz);
private:
  double x,y,z;
  double vx,vy,vz;
};

void NPC::move(double new_vx, double new_vy, double new_vz,
               double &res_x, double &res_y, double &res_z,
               double &res_vx, double &res_vy, double &res_vz)
{
  vx = new_vx;
  vy = new_vy;
  vz = new_vz;
  
  x += vx;
  y += vy;
  z += vz;

  res_x = x;
  res_y = y;
  res_z = z;

  res_vx = vx;
  res_vy = vy;
  res_vz = vz;
}

int main()
{
  double i;
  NPC npc1;
  double x,y,z, vx,vy,vz;
  TIME_ON;
  for(i=0; i<10000000.0; i+=1.0)
    npc1.move(i,i-1.0,i+1.0, x,y,z, vx,vy,vz);
  TIME_OFF("NPC movements");
  std::cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<std::endl;
  return 0;
}
