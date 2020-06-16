#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <SDL.h>

#include <list>
#include <cassert>
#include <iostream>

#define COLORSET_NONE colorset(0,0,0,0)

using namespace std;


const int TRACE_LEN = 20;



enum mouse_state
{
  _MOUSE_LEFT = 1,
  _MOUSE_RIGHT = 2,
  _MOUSE_MIDDLE = 4,
  _MOUSE_NONE = 0
};

enum control_events
{
  _EVENT_WINDOWS_CLOSED = 1,
  _EVENT_WINDOWS_RESIZED = 2,
  _EVENT_MOUSE_UNPRESSED = 4,
  
};


class colorset
{

  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
  friend int SDL_SetRenderDrawColor(SDL_Renderer *ren, colorset c);
public:
  colorset(Uint8 rn = 40, Uint8 gn = 40, Uint8 bn = 40, Uint8 an = 255) : r(rn), g(gn), b(bn), a(an) {}
  bool operator == (colorset const &x)
  {
    return (r == x.r) && (g == x.g) && (b == x.b) && (a == x.a);
  }
  colorset& operator +(const colorset& B)
  {
    colorset res;
    res.a = (a + B.a) % 256;
    res.r = (r + B.r) % 256;
    res.g = (g + B.g) % 256;
    res.b = (b + B.b) % 256;
    return res;
  }
  colorset& operator -(const colorset& B)
  {
    colorset res;
    res.a = (256 + a - B.a) % 256;
    res.r = (256 + r - B.r) % 256;
    res.g = (256 + g - B.g) % 256;
    res.b = (256 + b - B.b) % 256;
    return res;
  }
  colorset& operator +=(const colorset &B)
  {
    a = (a + B.a) % 256;
    r = (r + B.r) % 256;
    g = (g + B.g) % 256;
    b = (b + B.b) % 256;
    return *this;
  }
  colorset& operator *=(const double &mult)
  {
    a = (Uint8)(a * mult);
    r = (Uint8)(r * mult);
    g = (Uint8)(g * mult);
    b = (Uint8)(b * mult);
    return *this;
  }
  SDL_Color my_col_2_sdl(void)
  {
    SDL_Color res = { r, g, b, a };
    return res;
  }
};


class a_coordinates
{
  // is it ok tho?
  int x;
  int y;
  friend class r_coordinates;
  friend void SDL_GetWindowSize(SDL_Window *w, a_coordinates &sz);
  friend int SDL_RenderDrawLine(SDL_Renderer *ren, a_coordinates pt1, a_coordinates pt2);
  friend SDL_Rect InitSdlRect(a_coordinates pos, a_coordinates size);
  a_coordinates(double x, double y) : x((int)(x)), y((int)(y)) {}
public:
  a_coordinates(int x, int y) : x(x), y(y) {}
  a_coordinates(int xy = 0) : x(xy), y(xy) {}
  bool withinrange(const a_coordinates &range) const;
  bool withinrange(const a_coordinates &pos, const a_coordinates &range) const;
  double len(void);
  double r(const a_coordinates &second_dot);
  double get_angle();
  a_coordinates operator + (const a_coordinates &right)
  {
    return a_coordinates(this->x + right.x, this->y + right.y);
  }
  a_coordinates operator + (const double &right)
  {
    return a_coordinates(this->x + right, this->y + right);
  }
  a_coordinates operator - (const a_coordinates &right)
  {
    return a_coordinates(this->x - right.x, this->y - right.y);
  }
  a_coordinates operator - (const double &right)
  {
    return a_coordinates(this->x - right, this->y - right);
  }
  a_coordinates operator * (const double &right)
  {
    return a_coordinates(this->x * right, this->y * right);
  }
  a_coordinates& operator = (a_coordinates right)
  {
    x = right.x;
    y = right.y;
    return *this;
  }
  a_coordinates& operator += (const a_coordinates &right)
  {
    x += right.x;
    y += right.y;
    return *this;
  }
  a_coordinates& operator -= (const a_coordinates &right)
  {
    x -= right.x;
    y -= right.y;
    return *this;
  }
  bool operator == (const a_coordinates &right)
  {
    return (x==right.x)&&(y==right.y);
  }
};


class r_coordinates
{
// is it ok tho?
  double x;
  double y;
  friend a_coordinates;
public:
  r_coordinates(double x, double y) : x(x), y(y) {}
  r_coordinates(double xy = 0) : x(xy), y(xy) {}
  r_coordinates(const a_coordinates& pos, const int& W, const int& H);
  r_coordinates(const a_coordinates& pos, const a_coordinates& screen);
  a_coordinates rel2abs(const a_coordinates &screen);
  bool withinrange(const r_coordinates &range);
  bool withinrange(const r_coordinates &pos, const r_coordinates &range) const;
  bool withinrange_topleft(const r_coordinates &pos, const r_coordinates &range) const;
  double len(void);
  double r(const r_coordinates &second_dot);
  double get_angle();
  r_coordinates operator + (const r_coordinates &right)
  {
    return r_coordinates(this->x + right.x, this->y + right.y);
  }
  r_coordinates operator + (const double &right)
  {
    return r_coordinates(this->x + right, this->y + right);
  }
  r_coordinates operator - (const r_coordinates &right)
  {
    return r_coordinates(this->x - right.x, this->y - right.y);
  }
  r_coordinates operator - (const double &right)
  {
    return r_coordinates(this->x - right, this->y - right);
  }
  r_coordinates operator * (const double &right)
  {
    return r_coordinates(this->x * right, this->y * right);
  }
  r_coordinates operator ^ (const double &angle)
  {
    return r_coordinates(this->x * cos(angle) - this->y * sin(angle), this->x * sin(angle) + this->y * cos(angle));
  }
  r_coordinates& operator = (r_coordinates right)
  {
    x = right.x;
    y = right.y;
    return *this;
  }
  r_coordinates& operator += (const r_coordinates &right)
  {
    x += right.x;
    y += right.y;
    return *this;
  }
  r_coordinates& operator -= (const r_coordinates &right)
  {
    x -= right.x;
    y -= right.y;
    return *this;
  }
  r_coordinates& operator ^= (const double &angle)
  {
    this->x = this->x * cos(angle) - this->y * sin(angle);
    this->y = this->x * sin(angle) + this->y * cos(angle);
  }
  r_coordinates& invY(void)
  {
    y = -y;
    return *this;
  }
};

class scene_object;
class line;
class line_trace;

class scene
{
  SDL_Window* win;
  SDL_Renderer* ren;
  list <scene_object*> entry;
  a_coordinates Size;
  r_coordinates screen_limits;
public:
  unsigned long long event_flags;
  void UpdateScreenLimits(void);
  r_coordinates GetScreenLimits(void);
  r_coordinates GetMousePos(void);
  r_coordinates GetMousePosAndState(int &mouse_button_state);
  scene(int w, int h);
  void DrawLine(colorset c, r_coordinates pt1, r_coordinates pt2, int lineWidth);
  void DrawRectangle(colorset c, r_coordinates leftBottomCorner, r_coordinates size);
  void DrawText(string T, colorset c, r_coordinates leftBottomCorner, r_coordinates size);
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
    SDL_DestroyWindow(win);
    SDL_Quit();
  }
};

class scene_object
{
protected:
  r_coordinates pos;
  r_coordinates speed;
  double angle;
  double aspeed;
public:
  virtual void draw(scene& env) = 0;
  virtual bool iscollided(scene& env, double dt) = 0;
  virtual void update(scene& env, double dt) = 0;
  virtual ~scene_object(void) = default;
};


class rectangle : public scene_object
{

protected:
  r_coordinates size;
  colorset col;
  colorset colH;
  bool highlighted;
public:
  rectangle(r_coordinates p, r_coordinates sz, colorset c, colorset cH)
  {
    pos = p;    size = sz;    col = c;    colH = cH;    
    angle = 0;    aspeed = 0;    speed = r_coordinates();
  }
  bool is_into_rect(const r_coordinates& point)
  {
    return point.withinrange_topleft(pos, size);
  }
  virtual bool iscollided(scene& env, double dt) override
  {
      return 0;
  }
  virtual void update(scene& env, double dt) override
  {
    highlighted = is_into_rect(env.GetMousePos());
  }
  virtual void draw(scene& env) override
  {
    env.DrawRectangle(highlighted ? colH : col, pos, size);
  }
};

class rectangle_with_text : public rectangle
{

protected:
  colorset tcol;
  colorset tcolH;
  string text;

public:
  rectangle_with_text(r_coordinates p, r_coordinates sz, colorset c, colorset cH, string t, colorset tc, colorset tcH) : rectangle(p, sz, c, cH)
  {
    tcol = tc;
    tcolH = tcH;
    text = t;
  }
  virtual void draw(scene& env) override
  {
    env.DrawRectangle(highlighted ? colH : col, pos, size);
    env.DrawText(text, highlighted ? tcolH : tcol, pos, size);

  }
};

class button_image : public rectangle_with_text
{
protected:
  int code;
  void (*callback_fun)(int x);
public:
  button_image(r_coordinates p, r_coordinates sz, colorset c, colorset cH, string t, colorset tc, colorset tcH, int cd, void(*f)(int x)) : rectangle_with_text(p, sz, c, cH, t, tc, tcH)
  {
    code = cd;
    callback_fun = f;
  }
  virtual void update(scene& env, double dt) override
  {
    int state;
    highlighted = is_into_rect(env.GetMousePosAndState(state));
    if ((env.event_flags & _EVENT_MOUSE_UNPRESSED) && highlighted && callback_fun != nullptr)
    {
      callback_fun(code);
      env.event_flags &= (!_EVENT_MOUSE_UNPRESSED);
    }
  }
};

class line_trace : public scene_object
{
  class one__line_trace_object
  {
  public:

    r_coordinates pos;
    double angle;
    colorset col;
    double len;
    double width;

    one__line_trace_object(const line& parent);
    void blur(double widthMultiplier = 1.2, double alphaMultiplier = 0.88, double lenMultipllier = 1.1);
    void draw(scene &env);
  };

protected:
  list <one__line_trace_object> prints;

public:
  void addprint(const line& stepper);
  line_trace(const line& parent);
  virtual bool iscollided(scene& env, double dt) override
  {
    return 0;
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

  line(r_coordinates posI = r_coordinates(), double lenI = 0.1, int widthI = 1, r_coordinates speedI = r_coordinates(), double angleI = 0, double aspeedI = 0, colorset colI = colorset());
  ~line(void);

  virtual bool iscollided(scene& env, double dt) override;
  virtual void update(scene& env, double dt) override;
  virtual void draw(scene& env) override;
};
