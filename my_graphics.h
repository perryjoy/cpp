#pragma once


#define _USE_MATH_DEFINES
#include <cmath>
#include <SDL.h>
//#include <SDL_gfx>
#include <list>
#include <cassert>
#include <iostream>



using namespace std;


const int TRACE_LEN = 20;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 512;



#define EXPAND_COLORSET(C) (C).r, (C).g, (C).b, (C).a
#define EXPAND_COORDINATES(C, H, W) (C).x_int(H), (C).y_int(W)


typedef enum hit_by
{
    NONE = 0,
    TOP,
    LEFT,
    BOTTOM,
    RIGHT
} hit_by;

class colorset
{
public:
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;

  colorset(Uint8 rn = 40, Uint8 gn = 40, Uint8 bn = 40, Uint8 an = 255) : r(rn), g(gn), b(bn), a(an)
  {
  }
};

class coordinates
{
public: // is it ok tho?
  double x;
  double y;
  coordinates(double x, double y) : x(x), y(y)
  {
  }
  coordinates(double xy = 0) : x(xy), y(0)
  {
  }
  hit_by withinrange(const coordinates &range);
  int x_int(int W, int SZ)
  {
    return (int)round(x * SZ / 2.0 + W / 2.0);
  }
  int y_int(int H, int SZ)
  {
    return (int)round(- y * SZ / 2.0 + H / 2.0);
  }
  coordinates operator + (const coordinates &right)
  {
    return coordinates(this->x + right.x, this->y + right.y);
  }

  double operator / (const coordinates& right) //vector multiplication
  {
      return (this->x * right.y - this->y * right.x);
  }
  double operator * (const coordinates& right) // scalar multiplication
  {
      return (this->x * right.x + this->y * right.y);
  }

  coordinates operator + (const double &right)
  {
    return coordinates(this->x + right, this->y + right);
  }
  coordinates operator - (const coordinates &right)
  {
    return coordinates(this->x - right.x, this->y - right.y);
  }
  coordinates operator - (const double &right)
  {
    return coordinates(this->x - right, this->y - right);
  }
  coordinates operator * (const double &right)
  {
    return coordinates(this->x * right, this->y * right);
  }
  coordinates operator ^ (const double &angle)
  {
    return coordinates(this->x * cos(angle) - this->y * sin(angle), this->x * sin(angle) + this->y * cos(angle));
  }

  coordinates& operator = (coordinates right)
  {
    x = right.x;
    y = right.y;
    return *this;
  }
  coordinates& operator += (const coordinates &right)
  {
    x += right.x;
    y += right.y;
    return *this;
  }
  coordinates& operator -= (const coordinates &right)
  {
    x -= right.x;
    y -= right.y;
    return *this;
  }
  coordinates& operator ^= (const double &angle)
  {
    this->x = this->x * cos(angle) - this->y * sin(angle);
    this->y = this->x * sin(angle) + this->y * cos(angle);
  }
};

class scene_object;
class line;
class line_trace;

class scene
{
  SDL_Renderer* ren;
  list <scene_object*> entry;
  int height;
  int width;
  coordinates screen_limits;
public:
  void UpdateScreenLimits(void);
  coordinates GetScreenLimits(void);
  scene(SDL_Window *w);
  void DrawLine(colorset c, coordinates pt1, coordinates pt2, int lineWidth);
  SDL_Renderer * GetRen(void);
  void PushObject(scene_object* obj);
  scene_object* PopObject(scene_object *toRemove);
  void DeleteFrontObject(void);
  void DrawScene(void);
  int ScreenSize(void);
  ~scene(void)
  {
    while (!entry.empty())
    {
      DeleteFrontObject();
    }
    SDL_DestroyRenderer(ren);
  }
};

class scene_object
{
protected:
  coordinates pos;
  coordinates speed;
  double angle;
  double aspeed;
public:
  virtual void draw(scene& env) = 0;
  virtual hit_by iscollided(scene& env, double dt) = 0;
  virtual void update(scene& env, double dt) = 0;
  virtual ~scene_object(void) = default;
};


class line_trace : public scene_object
{
  class one__line_trace_object
  {
  public:

    coordinates pos;
    double angle;
    colorset col;
    double len;
    double width;

    one__line_trace_object(const line& parent);
    void blur(double widthMultiplier = 1.25, double alphaMultiplier = 0.9, double lenMultipllier = 1.05);
    void draw(scene &env);
  };

protected:
  list <one__line_trace_object> prints;
  //line_trace(coordinates posI = coordinates(), double lenI = 0.1, int widthI = 1, coordinates speedI = coordinates(), double angleI = 0, double aspeedI = 0, colorset colI = colorset())
  //{
  //  pos = posI;
  //  len = lenI;
  //  width = widthI;
  //  speed = speedI;
  //  angle = angleI;
  //  aspeed = aspeedI;
  //  col = colI;
  //}
public:
  void addprint(const line& stepper);
  line_trace(const line& parent);
  virtual hit_by iscollided(scene& env, double dt) override
  {
    return NONE;
  }
  virtual void update(scene& env, double dt) override
  {
    for (auto& one : prints)
      one.blur();
  }
  virtual void draw(scene& env) override
  {
    for (auto& one : prints)
      one.draw(env);
  }

};


class line : public scene_object
{
  friend class line_trace;

protected:
  colorset col;
  double len;
  int width;
  line_trace *tr;

public:

  line(coordinates posI = coordinates(), double lenI = 0.1, int widthI = 1, coordinates speedI = coordinates(), double angleI = 0, double aspeedI = 0, colorset colI = colorset());
  ~line(void);

  virtual hit_by iscollided(scene& env, double dt) override
  {
    coordinates next = pos + (speed*dt);
    double anext = angle + aspeed * dt;
    return (hit_by)((next + (coordinates(len) ^ anext)).withinrange(env.GetScreenLimits()) + (next - (coordinates(len) ^ anext)).withinrange(env.GetScreenLimits())); //dangerous but simple
  }
  
  virtual void update(scene& env, double dt) override
  {
    static double dtTrace = 0;

    if (iscollided(env, dt) == NONE || iscollided(env, dt) > 4)
    {
      pos += (speed * dt);
      angle += aspeed * dt;
      if (angle >= M_PI)
          angle -= M_PI;
    }
    else
    {
      coordinates n, e;
      coordinates V1, dV;
      double V2;
      hit_by hit = iscollided(env, dt);
      
      switch (hit)
      {
      case LEFT:
          n = coordinates(1, 0);
          e = coordinates(1, 0) ^ (angle > M_PI_2 ? angle - M_PI : angle);
          break;
      case RIGHT:
          n = coordinates(-1, 0);
          e = coordinates(1, 0) ^ (angle > M_PI_2 ? angle : angle - M_PI);
          break;
      case TOP:
          n = coordinates(0, -1);
          e = coordinates(1, 0) ^ (angle - M_PI);
          break;
      case BOTTOM:
          n = coordinates(0, 1);
          e = coordinates(1, 0) ^ angle;
          break;
      default:
          exit(-1);
          break;
      }

      
      V1 = coordinates(e.y * aspeed, -e.x * aspeed);
      V2 = ((speed + (V1 * len)) * n);
      dV = n * (-V2);
      //V2 = ((speed * speed) + (aspeed * aspeed * len * len)) / (((speed+dV) * (speed+dV)) + ((aspeed + (dV / e)) * (aspeed + (dV / e))));
      dV = dV * (1 + (n * e));
      //V2 = sqrt((speed * speed) + (aspeed * aspeed * len * len) - ((speed + dV) * (speed + dV)));
      aspeed = ((dV / e) > 0 ? 1 : -1) * sqrt((speed * speed) + (aspeed * aspeed * len * len) - ((speed + dV) * (speed + dV))) / len;
      speed += dV;
      //aspeed += ((dV / e) / len * 2);
    }

    dtTrace += dt;
    if (dtTrace > 0.1)
    {
      dtTrace -= 0.1;
      tr->addprint(*this);
      tr->update(env, dt);
      col.r = (col.r + 10) % 255;
      col.g = (col.g + 20) % 255;
      col.b = (col.b + 30) % 255;
    }
    //col.r = (col.r + 10) % 255;
    //col.g = (col.g + 20) % 255;
    //col.b = (col.b + 30) % 255;
  }

  virtual void draw(scene& env) override
  {
    tr->draw(env);
    env.DrawLine(col, pos + (coordinates(len) ^ angle), pos - (coordinates(len) ^ angle), width);
    //SDL_RenderDrawPoint(env.GetRen(), pos.x_int(width, env.ScreenSize()), pos.y_int(height, env.ScreenSize())); TEST
  }
};







