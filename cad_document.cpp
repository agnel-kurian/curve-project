#include "cad_primitives.h"
#include "cad_document.h"
#include "cad_gui_view.h"

namespace cad_core {

template class point_2d<float>;
template class line_2d<float>;
template class polyline_2d<float>;
template class cad_document<float>;
template class cad_gui_view<float>;

template class point_2d<double>;
template class line_2d<double>;
template class polyline_2d<double>;
template class cad_document<double>;
template class cad_gui_view<double>;

}
