#include "stretchy_container.hpp"

StretchyContainer::StretchyContainer()
{
    set_mouse_filter(MOUSE_FILTER_PASS);
    reset_function();
    resize_x = false;
    resize_y = false;
}
StretchyContainer::~StretchyContainer()
{}


void StretchyContainer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("resize_child_by"), &StretchyContainer::resize_child_by);

    ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &StretchyContainer::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &StretchyContainer::get_enabled);

    ClassDB::bind_method(D_METHOD("set_duration","duration"), &StretchyContainer::set_duration);
    ClassDB::bind_method(D_METHOD("get_duration"), &StretchyContainer::get_duration);

    ClassDB::bind_method(D_METHOD("get_stretch_curve_x"), &StretchyContainer::get_stretch_curve_x);
    ClassDB::bind_method(D_METHOD("set_stretch_curve_x", "stretch_curve_x"), &StretchyContainer::set_stretch_curve_x);
    ClassDB::bind_method(D_METHOD("get_stretch_curve_y"), &StretchyContainer::get_stretch_curve_y);
    ClassDB::bind_method(D_METHOD("set_stretch_curve_y", "stretch_curve_y"), &StretchyContainer::set_stretch_curve_y);
    
    ClassDB::bind_method(D_METHOD("get_stretch_origin"), &StretchyContainer::get_stretch_origin);
    ClassDB::bind_method(D_METHOD("set_stretch_origin", "stretch_origin"), &StretchyContainer::set_stretch_origin);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "get_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT,"duration"),"set_duration","get_duration");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "stretch_origin", PROPERTY_HINT_ENUM, "Centre,Left,Right,Top,Bottom"), "set_stretch_origin", "get_stretch_origin");
    ADD_GROUP("Stretch Curves", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stretch_curve_x", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_stretch_curve_x", "get_stretch_curve_x");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stretch_curve_y", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_stretch_curve_y", "get_stretch_curve_y");
}
PackedInt32Array StretchyContainer::_get_allowed_size_flags_horizontal() const
{
    PackedInt32Array flags;
    flags.append(SIZE_FILL);
    flags.append(SIZE_SHRINK_BEGIN);
    flags.append(SIZE_SHRINK_CENTER);
    flags.append(SIZE_SHRINK_END);
    return flags;
}
PackedInt32Array StretchyContainer::_get_allowed_size_flags_vertical() const 
{
    PackedInt32Array flags;
    flags.append(SIZE_FILL);
    flags.append(SIZE_SHRINK_BEGIN);
    flags.append(SIZE_SHRINK_CENTER);
    flags.append(SIZE_SHRINK_END);
    return flags;
}
//Needed to get min size of children in order to nest this to other containers
Vector2 StretchyContainer::_get_minimum_size() const
{
    if (get_child_count() < 1) return Vector2();

    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree()) return Vector2();
    if (c->is_set_as_top_level()) return Vector2();

    return c->get_combined_minimum_size();
}
void StretchyContainer::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_CANVAS:
        {
            still_inside = false;
            reset_tweens();
        }break;
        case NOTIFICATION_SORT_CHILDREN:
        {
            resize_child_by(0.0f);
        } break;
        case NOTIFICATION_RESIZED:
        {
            resize_child_by(0.0f);
        } break;
        case NOTIFICATION_MOUSE_ENTER:
        {
            if(!still_inside && enabled)
                pop_box();
        }break;
        case NOTIFICATION_MOUSE_EXIT:
        {
            if (0.0f < get_local_mouse_position().x && get_local_mouse_position().x < get_size().x
                && 0.0f < get_local_mouse_position().y && get_local_mouse_position().y < get_size().y)
            {
                still_inside = true;
            }
            else
            {
                still_inside = false;
                if (enabled)
                    unpop_box();
            }

        }break;
        case NOTIFICATION_EXIT_CANVAS:
        {
            unpop_tw->unreference();
            pop_tw->unreference();
        }break;
	}
}
void StretchyContainer::reset_function()
{
    switch (stretch_origin)
    {
        WARN_PRINT(String::num_int64(stretch_origin));
        case(STRETCH_ORIGIN_CENTER):
        {
            _resize_child_by = &StretchyContainer::offset_from_center;
        }break;
        case(STRETCH_ORIGIN_LEFT):
        {
            _resize_child_by = &StretchyContainer::offset_from_left;
        }break;
        case(STRETCH_ORIGIN_RIGHT):
        {
            _resize_child_by = &StretchyContainer::offset_from_right;
        }break;
        case(STRETCH_ORIGIN_TOP):
        {
            _resize_child_by = &StretchyContainer::offset_from_top;
        }break;
        case(STRETCH_ORIGIN_BOTTOM):
        {
            _resize_child_by = &StretchyContainer::offset_from_bottom;
        }break;
        default:
        {
            _resize_child_by = &StretchyContainer::offset_from_center;
        }break;
    }
}
void StretchyContainer::reset_tweens()
{
    pop_tw = this->create_tween();
    unpop_tw = this->create_tween();
    pop_tw->connect("finished", Callable(pop_tw.ptr(), "stop"));//Prevents the tween becoming disabled when it finishes
    unpop_tw->connect("finished", Callable(unpop_tw.ptr(), "stop"));

    pop_tw->tween_method(Callable(this, "resize_child_by"), 0.0f, 1.0f, duration);
    unpop_tw->tween_method(Callable(this, "resize_child_by"), 1.0f, 0.0f, duration);
   
    pop_tw->stop();
    unpop_tw->stop();


    if (stretch_curve_x.is_valid()){resize_x = true;}
    else{resize_x = false;}

    if (stretch_curve_y.is_valid()){ resize_y = true; }
    else{ resize_y = false; }
}
void StretchyContainer::resize_child_by(const float amm)
{
    if (get_child_count() < 1) return;
    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree() ||
        c->is_set_as_top_level()) return;

    Vector2 size = get_size();
   
    Vector2 child_size = size;
    child_size.x = resize_x ? size.x * stretch_curve_x->sample(amm) : size.x;
    child_size.y = resize_y ? size.y * stretch_curve_y->sample(amm) : size.y;

    Point2 ofs = (*_resize_child_by)(size, child_size);

    fit_child_in_rect(c, Rect2(ofs, child_size));
}
Point2 StretchyContainer::offset_from_center(const Vector2& size, const Vector2& child_size)
{
    return ((size - child_size) / 2.0).floor();
}
Point2 StretchyContainer::offset_from_left(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.y = int((size.y - child_size.y) / 2.0);
    return ofs;
}
Point2 StretchyContainer::offset_from_right(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.x = int(size.x - child_size.x);
    ofs.y = int((size.y - child_size.y) / 2.0);
    return ofs;
}
Point2 StretchyContainer::offset_from_top(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.x = int((size.x - child_size.x) / 2.0);
    return ofs;
}
Point2 StretchyContainer::offset_from_bottom(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.y = int(size.y - child_size.y);
    ofs.x = int((size.x - child_size.x) / 2.0);
    return ofs;
}
void StretchyContainer::pop_box()
{
    double elapsed_time = 0;
    if (unpop_tw->is_running())
        elapsed_time = unpop_tw->get_total_elapsed_time();

    unpop_tw->stop();
    pop_tw->stop();
    pop_tw->custom_step(duration - elapsed_time);
    pop_tw->play();
}
void StretchyContainer::unpop_box()
{
    double elapsed_time = 0;
    if(pop_tw->is_running())
        elapsed_time = pop_tw->get_total_elapsed_time();

    pop_tw->stop();
    unpop_tw->stop();
    unpop_tw->custom_step(duration  - elapsed_time);
    unpop_tw->play();
}
void StretchyContainer::set_enabled(const bool enb) { enabled = enb; }
bool StretchyContainer::get_enabled() const { return enabled; }

void StretchyContainer::set_duration(const float dur) { duration = dur; }
float StretchyContainer::get_duration() const { return duration; }

Ref<Curve> StretchyContainer::get_stretch_curve_x() const { return stretch_curve_x; }
void StretchyContainer::set_stretch_curve_x(const Ref<Curve> cve)
{
    stretch_curve_x = cve;
    if (stretch_curve_x.is_valid()) { resize_x = true; }
    else { resize_x = false; }
    resize_child_by(0.0f); 
}
Ref<Curve> StretchyContainer::get_stretch_curve_y() const { return stretch_curve_y; }

void StretchyContainer::set_stretch_curve_y(const Ref<Curve> cve)
{
    stretch_curve_y = cve;
    if (stretch_curve_y.is_valid()) { resize_y = true; }
    else { resize_y = false; }
    resize_child_by(0.0f);
}

void StretchyContainer::set_stretch_origin(uint32_t str) { 
    stretch_origin = static_cast<StretchOrigin>(str);
    reset_function();
    resize_child_by(0.0f);
}
uint32_t StretchyContainer::get_stretch_origin() const { return stretch_origin; }