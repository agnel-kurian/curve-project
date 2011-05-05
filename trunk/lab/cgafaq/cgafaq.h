#ifndef __CGAFAQ_H__

#define __CGAFAQ_H__

#include <vector>

namespace cgafaq {
  template<typename T> struct P2 {
    T x;
    T y;
    
    P2(const T& x, const T& y) : x(x), y(y) {}
    P2(const P2& other) : x(other.x), y(other.y) {}
  };

  template<typename T> struct types {
    typedef std::vector< struct P2<T> > polygon;
  };
  
  template<typename T> void intersect(T* r, T* s,
    const P2<T>& a, const P2<T>& b, const P2<T>& c, const P2<T>& d){
      
    T denom = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);
      
    *r = ((a.y - c.y) * (d.x - c.x) - (a.x - c.x) * (d.y - c.y)) / denom;
    *s = ((a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y)) / denom;
  }
  
  template<typename T>
  void distance_from_line(T* r, T* s,
    const P2<T>& a, const P2<T>& b, const P2<T>& c){
    
    T dx = b.x - a.x;
    T dy = b.y - a.y;
      
    T l2 = dx * dx + dy * dy;
    
    *r = ((a.y - c.y) * (a.y - b.y) - (a.x - c.x) * (b.x - a.x)) / l2;
    *s = ((a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y)) / l2;
  }
  
  template<typename T>
  int lower_right(const typename types<T>::polygon& polygon){
      int m = 0;

      P2<T> p0 = polygon[0];

      P2<T> min = p0;

      for(int i = 0, n = polygon.size(); i < n; i++) {
        P2<T> pi = polygon[i];
        if((pi.y < min.y) || ((pi.y == min.y) && (pi.x > min.x))) {
          m = i;
          min.x = pi.x;
          min.y = pi.y;
        }
      }
      return m;
  }
  
  template<typename T>
  T orientation(const P2<T>& a, const P2<T>& b,
    const P2<T>& c){
      
    return a.x * b.y - a.y * b.x +
      a.y * c.x - a.x * c.y +
      b.x * c.y - c.x * b.y;
      
  }
  
  template<typename T>
  T orientation(const typename types<T>::polygon& polygon){
      int m = lower_right(polygon);

      int n = polygon.size();
      int m1 = (m + (n - 1)) % n; 
      P2<T>& a = polygon[m1];
      P2<T>& b = polygon[m];
      P2<T>& c = polygon[(m + 1) % n];

      return orientation(a, b, c);
  }
  
  //	the following is based on code from
  //	http://www.cgal.org/Manual/3.4/include/CGAL/constructions/kernel_ftC2.h
  template<typename T>	
  void line_equation(const P2<T>& p, const P2<T>& q, T* a, T* b, T* c){
    // The horizontal and vertical line get a special treatment
    // in order to make the intersection code robust for doubles 
    if(p.y == q.y){
      *a = 0 ;
      if(q.x > p.x){
        *b = 1;
        *c = -p.y;
      } else if(q.x == p.x){
        *b = 0;
        *c = 0;
      }else{
        *b = -1;
        *c = p.y;
      }
    } else if(q.x == p.x){
      *b = 0;
      if(q.y > p.y){
        *a = -1;
        *c = p.x;
      } else if (q.y == p.y){
        *a = 0;
        *c = 0;
      } else {
        *a = 1;
        *c = -p.x;
      }
    } else {
      *a = p.y - q.y;
      *b = q.x - p.x;
      *c = -p.x * *a - p.y * *b;
    }
  }
  
  template<typename T>
  T point_on_line(const P2<T>& p,
    const T& a, const T& b, const T& c){
      
    return a * p.x + b * p.y + c;
  }

  template<typename T>
  P2<T> perpendicular_cw(const P2<T>& from, const P2<T>& to){
    T dx = to.x - from.x;
    T dy = to.y - from.y;
    
    return P2<T>(to.x + dy, to.y - dx);
  }
  
  template<typename T>
  P2<T> perpendicular_ccw(const P2<T>& from, const P2<T>& to){
    T dx = to.x - from.x;
    T dy = to.y - from.y;
    
    return P2<T>(to.x + dy, to.y - dx);
  }
  
  
  template<typename T>
  bool point_in_polygon(const typename types<T>::polygon& polygon,
    const P2<T>& p){

    int nverts = polygon.size();
    bool c = false;
    for (int i = 0, j = nverts - 1; i < nverts; j = i++) {
      const P2<T>& pi = polygon[i];
      const P2<T>& pj = polygon[j];
      
      if ((((pi.y <= p.y) && (p.y < pj.y)) ||
        ((pj.y <= p.y) && (p.y < pi.y))) &&
        (p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x))
        c = !c;
    }
    
    return c;

  }
}

#endif
