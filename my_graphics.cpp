#include "my_graphics.h"

/// coord

hit_by coordinates::withinrange(const coordinates &range)
{
  assert(range.x >= 0 && range.y >= 0);
  if (x < -range.x)
      return LEFT;
  if (x > range.x)
      return RIGHT;
  if (y < -range.y)
      return BOTTOM;
  if (y > range.y)
      return TOP;
  return NONE;
}

/// scene


scene::scene(SDL_Window *w)
{
  ren = SDL_CreateRenderer(w, -1, 0);
  SDL_GetWindowSize(w, &width, &height);
  UpdateScreenLimits();
}
void scene::DeleteFrontObject(void)
{
  delete entry.front();
  entry.pop_front();
}
void scene::DrawScene(void)
{
  static double tStart = SDL_GetTicks() / 1000.0;
  static double tFinish = SDL_GetTicks() / 1000.0;
  SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
  SDL_RenderClear(ren);
  tStart = SDL_GetTicks() / 1000.0;
  for (auto obj : entry)
  {
    obj->update(*this, tStart - tFinish);
    obj->draw(*this);
  }
  tFinish = tStart;
}
void scene::DrawLine(colorset c, coordinates pt1, coordinates pt2, int lineWidth)
{
  int pixelScale = ScreenSize();
  int x1 = pt1.x_int(width, pixelScale);
  int x2 = pt2.x_int(width, pixelScale);
  int y1 = pt1.y_int(height, pixelScale);
  int y2 = pt2.y_int(height, pixelScale);

  Sint16 dx = x2 - x1, dy = y2 - y1;
  /* Всегда рисуем основную линию */
  SDL_SetRenderDrawColor(ren, EXPAND_COLORSET(c));
  SDL_RenderDrawLine(ren, x1, y1, x2, y2);
  if (lineWidth > 1 && !(dx == 0 && dy == 0))
  {
    double r = (double)lineWidth / 2; /* радиус "кисти" для рисования */
    double phi; /* направляющий угол */
    double ri; /* для рисования параллельных линий */

    if (dy == 0)
      phi = dx > 0 ? 0 : M_PI;
    else
      if (dx == 0)
        phi = dy > 0 ? M_PI_2 : M_PI + M_PI_2;
      else
        phi = acos(dx / sqrt(dx*dx + dy * dy));
    if (dy > 0)
      phi = 2 * M_PI - phi;
    for (ri = 0; ri < r; ri += 0.5)
    { /* рисуем линии, параллельные исходной */
      Sint16 px1, py1, px2, py2;
      px1 = floor(x1 + ri * cos(phi + M_PI_2));
      py1 = floor(y1 - ri * sin(phi + M_PI_2));
      px2 = floor(x2 + ri * cos(phi + M_PI_2));
      py2 = floor(y2 - ri * sin(phi + M_PI_2));
      SDL_RenderDrawLine(ren, px1, py1, px2, py2);
      px1 = floor(x1 + ri * cos(phi - M_PI_2));
      py1 = floor(y1 - ri * sin(phi - M_PI_2));
      px2 = floor(x2 + ri * cos(phi - M_PI_2));
      py2 = floor(y2 - ri * sin(phi - M_PI_2));
      SDL_RenderDrawLine(ren, px1, py1, px2, py2);
    }
  }
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
  entry.remove_if([toRemove](scene_object *o) {return o == toRemove; });
  return toRemove;
}
void scene::UpdateScreenLimits(void)
{
  if (width > height)
    screen_limits = coordinates(1, (double)height / width);
  else
    screen_limits = coordinates((double)width / height, 1);
}
coordinates scene::GetScreenLimits(void)
{
  return screen_limits;
}
int scene::ScreenSize(void)
{
  return width > height ? width : height;
}

/// line


line::line(coordinates posI, double lenI, int widthI, coordinates speedI, double angleI, double aspeedI, colorset colI)
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

  int k = (int)(alphaMultiplier * 10000);

  col.a = col.a * k / 10000;
  col.r = col.r * k / 10000;
  col.g = col.g * k / 10000;
  col.b = col.b * k / 10000;

  //col.a = (Uint8)(col.a * alphaMultiplier);
  //col.a = (Uint8)(col.a * alphaMultiplier);
  //col.r = (Uint8)(col.b * alphaMultiplier);
  //col.g = (Uint8)(col.b * alphaMultiplier);
  //col.b = (Uint8)(col.b * alphaMultiplier);
}
void line_trace::one__line_trace_object::draw(scene &env)
{
  env.DrawLine(col, pos + (coordinates(len) ^ angle), pos - (coordinates(len) ^ angle), (int)width);
  SDL_SetRenderDrawColor(env.GetRen(), 255, 255, 255, 255);
//  SDL_RenderDrawPoint(env.GetRen(), pos.x_int(width, env.ScreenSize()), pos.y_int(height, env.ScreenSize())); TEST
}