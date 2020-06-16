#include "my_graphics.h"
#include <SDL_ttf.h>


/// coord




bool a_coordinates::withinrange(const a_coordinates &range) const
{
  return (x <= range.x && y <= range.y);
}
bool a_coordinates::withinrange(const a_coordinates &pos, const a_coordinates &range) const
{
  return (x >= pos.x && y >= pos.y && x <= pos.x + range.x && y <= pos.y + range.y);
}
double a_coordinates::len(void)
{
  return sqrt((double)(x * x + y * y));
}
double a_coordinates::r(const a_coordinates &second_dot)
{
  return sqrt((double)(x - second_dot.x)*(x - second_dot.x) + (y - second_dot.y)*(y - second_dot.y));
}
double a_coordinates::get_angle()
{
  double phi;
  if (y == 0)
    phi = x > 0 ? 0 : M_PI;
  else
    if (x == 0)
      phi = y > 0 ? M_PI_2 : M_PI + M_PI_2;
    else
    {
      phi = acos(x / sqrt(x*x + y*y));
      if (y > 0)
        phi = 2 * M_PI - phi;
    }
  return phi;
}



r_coordinates::r_coordinates(const a_coordinates& pos, const int& W, const int& H)
{
  int SZ = W > H ? W : H;
  this->x = (2.0 * pos.x - W) / SZ;
  this->y = (H - 2.0 * pos.y) / SZ;
}
r_coordinates::r_coordinates(const a_coordinates& pos, const a_coordinates& screen)
{
  int SZ = screen.x > screen.y ? screen.x : screen.y;
  this->x = (2.0 * pos.x - screen.x) / SZ;
  this->y = (screen.y - 2.0 * pos.y) / SZ;
}
a_coordinates r_coordinates::rel2abs(const a_coordinates &screen)
{
  int SZ = screen.x > screen.y ? screen.x : screen.y;
  a_coordinates res;
  res.x = (int)round(this->x * SZ / 2.0 + screen.x / 2.0);
  res.y = (int)round(-this->y * SZ / 2.0 + screen.y / 2.0);
  return res;
}
bool r_coordinates::withinrange(const r_coordinates &range) 
{
  assert(range.x >= 0 && range.y >= 0);
  return (fabs(x) <= range.x && fabs(y) <= range.y);
}
bool r_coordinates::withinrange(const r_coordinates &pos, const r_coordinates &range) const
{
  return (x >= pos.x && y >= pos.y && x <= pos.x + range.x && y <= pos.y + range.y);
}
bool r_coordinates::withinrange_topleft(const r_coordinates &pos, const r_coordinates &range) const
{
  return (x >= pos.x && y <= pos.y && x <= pos.x + range.x && y >= pos.y + range.y);
}
double r_coordinates::len(void)
{
  return sqrt(x * x + y * y);
}
double r_coordinates::r(const r_coordinates &second_dot)
{
  return sqrt((x - second_dot.x)*(x - second_dot.x) + (y - second_dot.y)*(y - second_dot.y));
}
double r_coordinates::get_angle()
{
  double phi;
  if (fabs(y) <= DBL_EPSILON)
    phi = x > 0 ? 0 : M_PI;
  else
    if (fabs(x) <= DBL_EPSILON)
      phi = y > 0 ? M_PI_2 : M_PI + M_PI_2;
    else
    {
      phi = acos(x / sqrt(x*x + y * y));
      if (y > 0)
        phi = 2 * M_PI - phi;
    }
  return phi;
}

/// scene

int SDL_SetRenderDrawColor(SDL_Renderer *ren, colorset c)
{
  return SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
}
void SDL_GetWindowSize(SDL_Window *w, a_coordinates &sz)
{
  SDL_GetWindowSize(w, &(sz.x), &(sz.y));
}
int SDL_RenderDrawLine(SDL_Renderer *ren, a_coordinates pt1, a_coordinates pt2)
{
  return SDL_RenderDrawLine(ren, pt1.x, pt1.y, pt2.x, pt2.y);
}

scene::scene(int w, int h)
{
  event_flags = 0;
  win = NULL;
  ren = NULL;
  Size = a_coordinates(w, h);
  UpdateScreenLimits();

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
  }
  else
  {
    win = SDL_CreateWindow("Karaev YG development", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (win == NULL)
    {
      cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    }
    else
    { 
      //SDL_UpdateWindowSurface(win);
      ren = SDL_CreateRenderer(win, -1, 0);
      //SDL_RenderPresent(ren);
    }
  }
}
void scene::DeleteFrontObject(void)
{
  delete entry.front();
  entry.pop_front();
}
void scene::DrawScene(void)
{
  SDL_Event event;
  static double tStart = SDL_GetTicks() / 1000.0;
  static double tFinish = SDL_GetTicks() / 1000.0;
  event_flags = 0;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
      {
        event_flags |= _EVENT_WINDOWS_RESIZED;
        Size = a_coordinates(event.window.data1, event.window.data2);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      event_flags |= _EVENT_MOUSE_UNPRESSED;
      break;
    case SDL_QUIT:
      event_flags |= _EVENT_WINDOWS_CLOSED;
      break;
    }
  }
  if (event_flags & _EVENT_WINDOWS_RESIZED)
  {

    //SDL_SetWindowSize(win, event.window.data1, event.window.data2);
    //SDL_UpdateWindowSurface(win);
    UpdateScreenLimits();
  }
  SDL_SetRenderDrawColor(ren, 100, 100, 100, 0);
  SDL_RenderClear(ren);
  tStart = SDL_GetTicks() / 1000.0;
  for (auto obj : entry)
  {
    obj->update(*this, tStart - tFinish);
    obj->draw(*this);
  }
  tFinish = tStart;  
  SDL_RenderPresent(ren);
}
void scene::DrawLine(colorset c, r_coordinates pt1, r_coordinates pt2, int lineWidth)
{
  a_coordinates p1 = pt1.rel2abs(Size);
  a_coordinates p2 = pt2.rel2abs(Size);

  a_coordinates dpt = p2-p1;
  /* Всегда рисуем основную линию */
  SDL_SetRenderDrawColor(ren, c);
  SDL_RenderDrawLine(ren, p1, p2);
  if (lineWidth > 1 && !(dpt == a_coordinates()))
  {
    double r = (double)lineWidth / 2; /* радиус "кисти" для рисования */
    double phi = dpt.get_angle(); /* направляющий угол */
    double ri; /* для рисования параллельных линий */

  
    for (ri = 0; ri < r; ri += 0.5)
    { /* рисуем линии, параллельные исходной */
      a_coordinates p1tmp, p2tmp;
      p1tmp = p1 + a_coordinates((int)floor(ri * cos(phi + M_PI_2)), (int)floor(-ri * sin(phi + M_PI_2)));
      p2tmp = p2 + a_coordinates((int)floor(ri * cos(phi + M_PI_2)), (int)floor(-ri * sin(phi + M_PI_2)));
      SDL_RenderDrawLine(ren, p1tmp, p2tmp);
      p1tmp = p1 + a_coordinates((int)floor(ri * cos(phi - M_PI_2)), (int)floor(-ri * sin(phi - M_PI_2)));
      p2tmp = p2 + a_coordinates((int)floor(ri * cos(phi - M_PI_2)), (int)floor(-ri * sin(phi - M_PI_2)));
      SDL_RenderDrawLine(ren, p1tmp, p2tmp);
    }
  }
}
SDL_Rect InitSdlRect(a_coordinates pos, a_coordinates size)
{

  SDL_Rect x = { pos.x, pos.y, size.x, size.y };
  return x;
}
void scene::DrawRectangle(colorset c, r_coordinates leftTopCorner, r_coordinates size)
{
  SDL_SetRenderDrawColor(ren, c);
  SDL_Rect r = InitSdlRect(leftTopCorner.rel2abs(Size), size.rel2abs(Size)-Size*0.5);
  SDL_RenderFillRect(ren, &r);
}
void scene::DrawText(string T, colorset c, r_coordinates leftTopCorner, r_coordinates size)
{
  TTF_Init();
  TTF_Font* Sans = TTF_OpenFont("OpenSans-Light.ttf", 100); //this opens a font style and sets a size
  SDL_Color C = c.my_col_2_sdl();
  SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, T.c_str(), C); 
  SDL_Texture* Message = SDL_CreateTextureFromSurface(ren, surfaceMessage); 

  SDL_Rect Message_rect = InitSdlRect(leftTopCorner.rel2abs(Size), size.rel2abs(Size) - Size * 0.5);;
  SDL_RenderCopy(ren, Message, NULL, &Message_rect); 

  TTF_CloseFont(Sans);
  SDL_FreeSurface(surfaceMessage);
  SDL_DestroyTexture(Message);
  TTF_Quit();
}
SDL_Renderer* scene::GetRen(void)
{
  return ren;
}
void scene::PushObject(scene_object* obj)
{
  entry.push_back(obj);
}
scene_object* scene::PopObject(scene_object *toRemove)
{
  if (toRemove != nullptr)
    entry.remove_if([toRemove](scene_object *o) {return o == toRemove; });
  return toRemove;
}
void scene::UpdateScreenLimits(void)
{
  double proportion = Size.get_angle();
  if (proportion > M_PI_4)
    screen_limits = r_coordinates(atan(proportion), 1);
  else
    screen_limits = r_coordinates(1, tan(proportion));
}
r_coordinates scene::GetScreenLimits(void)
{
  return screen_limits;
}
r_coordinates scene::GetMousePos(void)
{
  int x, y;

  SDL_GetMouseState(&x, &y);
  return r_coordinates(a_coordinates(x,y), this->Size);
}
r_coordinates scene::GetMousePosAndState(int &mouse_buttons_state)
{
  int x, y;

  mouse_buttons_state = int(SDL_GetMouseState(&x, &y));
  mouse_buttons_state = ((SDL_BUTTON(SDL_BUTTON_LEFT) & mouse_buttons_state) ? _MOUSE_LEFT : _MOUSE_NONE) |
                        ((SDL_BUTTON(SDL_BUTTON_RIGHT) & mouse_buttons_state) ? _MOUSE_RIGHT : _MOUSE_NONE) | 
                        ((SDL_BUTTON(SDL_BUTTON_MIDDLE) & mouse_buttons_state) ? _MOUSE_MIDDLE : _MOUSE_NONE);
  return r_coordinates (a_coordinates(x, y), this->Size);
}
int scene::ScreenSize(void)
{
  assert(false);
  return 0;
}

/// line


line::line(r_coordinates posI, double lenI, int widthI, r_coordinates speedI, double angleI, double aspeedI, colorset colI)
{
  pos = posI;
  len = lenI;
  width = widthI;
  speed = speedI;
  angle = angleI;
  aspeed = aspeedI;
  col = colI;
  tr = new line_trace(*this);
}
line::~line(void)
{
  delete tr;
}
bool line::iscollided(scene& env, double dt) 
{
  r_coordinates next = pos + (speed * dt);
  double anext = angle + aspeed * dt;
  return (next + (r_coordinates(len) ^ anext)).withinrange(env.GetScreenLimits()) &&
    (next - (r_coordinates(len) ^ anext)).withinrange(env.GetScreenLimits()) ? false : true;
}
void line::update(scene& env, double dt)
{
  static double dtTrace = 0;

  if (!iscollided(env, dt))
  {
    pos += (speed * dt);
    angle += aspeed * dt;
  }

  dtTrace += dt;
  if (dtTrace > 0.1)
  {
    dtTrace -= 0.1;
    tr->addprint(*this);
    tr->update(env, dt);
    col += colorset(10, 20, 30, 0);
  }

}

void line::draw(scene& env)
{
  tr->draw(env);
  env.DrawLine(col, pos + (r_coordinates(len) ^ angle), pos - (r_coordinates(len) ^ angle), width);
}



/// trace

void line_trace::addprint(const line& stepper)
{
  prints.push_back(one__line_trace_object(stepper));
  if (prints.size() > TRACE_LEN)
    prints.pop_front();
}
line_trace::line_trace(const line& parent)
{
  pos = parent.pos;
  angle = parent.angle;
  speed = 0;
  aspeed = 0;
  addprint(parent);
}

line_trace::one__line_trace_object::one__line_trace_object(const line& parent) :
  pos(parent.pos), angle(parent.angle), col(parent.col), len(parent.len), width((double)parent.width)
{
}

void line_trace::one__line_trace_object::blur(double widthMultiplier, double alphaMultiplier, double lenMultipllier)
{
  width *= widthMultiplier;
  len *= lenMultipllier;
  col *= alphaMultiplier;
}
void line_trace::one__line_trace_object::draw(scene &env)
{
  env.DrawLine(col, pos + (r_coordinates(len) ^ angle), pos - (r_coordinates(len) ^ angle), (int)width);
  SDL_SetRenderDrawColor(env.GetRen(), 255, 255, 255, 255);
//  SDL_RenderDrawPoint(env.GetRen(), pos.x_int(width, env.ScreenSize()), pos.y_int(height, env.ScreenSize())); TEST
}