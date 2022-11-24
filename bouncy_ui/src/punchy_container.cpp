#include "punchy_container.hpp"

PunchyContainer::PunchyContainer()
{
    set_mouse_filter(MOUSE_FILTER_PASS);
    reset_function();
}
PunchyContainer::~PunchyContainer()
{

}

void PunchyContainer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("resize_child_by"), &PunchyContainer::resize_child_by);
    ClassDB::bind_method(D_METHOD("unresize_child_by"), &PunchyContainer::unresize_child_by);

    ClassDB::bind_method(D_METHOD("is_mouse_entered"), &PunchyContainer::is_mouse_entered);
    ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &PunchyContainer::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &PunchyContainer::get_enabled);
    ClassDB::bind_method(D_METHOD("punch"), &PunchyContainer::punch);
    ClassDB::bind_method(D_METHOD("unpunch"), &PunchyContainer::unpunch);

    ClassDB::bind_method(D_METHOD("set_punch_duration", "punch_duration"), &PunchyContainer::set_punch_duration);
    ClassDB::bind_method(D_METHOD("get_punch_duration"), &PunchyContainer::get_punch_duration);
    ClassDB::bind_method(D_METHOD("set_unpunch_duration", "unpunch_duration"), &PunchyContainer::set_unpunch_duration);
    ClassDB::bind_method(D_METHOD("get_unpunch_duration"), &PunchyContainer::get_unpunch_duration);

    ClassDB::bind_method(D_METHOD("get_punch_curve_x"), &PunchyContainer::get_punch_curve_x);
    ClassDB::bind_method(D_METHOD("set_punch_curve_x", "punch_curve_x"), &PunchyContainer::set_punch_curve_x);
    ClassDB::bind_method(D_METHOD("get_unpunch_curve_x"), &PunchyContainer::get_unpunch_curve_x);
    ClassDB::bind_method(D_METHOD("set_unpunch_curve_x", "unpunch_curve_x"), &PunchyContainer::set_unpunch_curve_x);
    ClassDB::bind_method(D_METHOD("get_punch_curve_y"), &PunchyContainer::get_punch_curve_y);
    ClassDB::bind_method(D_METHOD("set_punch_curve_y", "punch_curve_y"), &PunchyContainer::set_punch_curve_y);
    ClassDB::bind_method(D_METHOD("get_unpunch_curve_y"), &PunchyContainer::get_unpunch_curve_y);
    ClassDB::bind_method(D_METHOD("set_unpunch_curve_y", "punch_curve_y"), &PunchyContainer::set_unpunch_curve_y);

    ClassDB::bind_method(D_METHOD("get_punch_origin"), &PunchyContainer::get_punch_origin);
    ClassDB::bind_method(D_METHOD("set_punch_origin", "punch_origin"), &PunchyContainer::set_punch_origin);

    ClassDB::bind_method(D_METHOD("emit_punch_finished"), &PunchyContainer::emit_punch_finished);
    ClassDB::bind_method(D_METHOD("emit_unpunch_finished"), &PunchyContainer::emit_unpunch_finished);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "get_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "punch_duration"), "set_punch_duration", "get_punch_duration");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "unpunch_duration"), "set_unpunch_duration", "get_unpunch_duration");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "punch_origin", PROPERTY_HINT_ENUM, "Centre,Left,Right,Top,Bottom"), "set_punch_origin", "get_punch_origin");
    ADD_GROUP("Punch Curves", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "punch_curve_x", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_punch_curve_x", "get_punch_curve_x");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "unpunch_curve_x", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_unpunch_curve_x", "get_unpunch_curve_x");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "punch_curve_y", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_punch_curve_y", "get_punch_curve_y");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "unpunch_curve_y", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_unpunch_curve_y", "get_unpunch_curve_y");

    ADD_SIGNAL(MethodInfo("punch_finished", PropertyInfo(Variant::OBJECT, "object")));
    ADD_SIGNAL(MethodInfo("unpunch_finished", PropertyInfo(Variant::OBJECT, "object")));
}
PackedInt32Array PunchyContainer::_get_allowed_size_flags_horizontal() const
{
    PackedInt32Array flags;
    flags.append(SIZE_FILL);
    flags.append(SIZE_SHRINK_BEGIN);
    flags.append(SIZE_SHRINK_CENTER);
    flags.append(SIZE_SHRINK_END);
    return flags;
}
PackedInt32Array PunchyContainer::_get_allowed_size_flags_vertical() const
{
    PackedInt32Array flags;
    flags.append(SIZE_FILL);
    flags.append(SIZE_SHRINK_BEGIN);
    flags.append(SIZE_SHRINK_CENTER);
    flags.append(SIZE_SHRINK_END);
    return flags;
}
//Needed to get min size of children in order to nest this to other containers
Vector2 PunchyContainer::_get_minimum_size() const
{
    if (get_child_count() < 1) return Vector2();

    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree()) return Vector2();
    if (c->is_set_as_top_level()) return Vector2();

    return c->get_combined_minimum_size();
}
void PunchyContainer::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_POSTINITIALIZE:
        {
            still_inside = false;
            resize_x = false;
            resize_y = false;
            mouse_entered = false;
        }break;
        case NOTIFICATION_READY:
        {
            still_inside = false;
            reset();
        }break;
        case NOTIFICATION_SORT_CHILDREN:
        {
            reset();
            resize_child_by(0.0f);
        } break;
        case NOTIFICATION_RESIZED:
        {
            reset();
            resize_child_by(0.0f);
        } break;
        case NOTIFICATION_MOUSE_ENTER:
        {
            if(!still_inside)
                mouse_entered = true;
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
                mouse_entered = false;
            }
        }break;
    }
}
void PunchyContainer::reset_function()
{
    switch (punch_origin)
    {
    case(PUNCH_ORIGIN_CENTER):
    {
        _resize_child_by = &PunchyContainer::offset_from_center;
    }break;
    case(PUNCH_ORIGIN_LEFT):
    {
        _resize_child_by = &PunchyContainer::offset_from_left;
    }break;
    case(PUNCH_ORIGIN_RIGHT):
    {
        _resize_child_by = &PunchyContainer::offset_from_right;
    }break;
    case(PUNCH_ORIGIN_TOP):
    {
        _resize_child_by = &PunchyContainer::offset_from_top;
    }break;
    case(PUNCH_ORIGIN_BOTTOM):
    {
        _resize_child_by = &PunchyContainer::offset_from_bottom;
    }break;
    default:
    {
        _resize_child_by = &PunchyContainer::offset_from_center;
    }break;
    }
}
void PunchyContainer::reset()
{
    if (punch_curve_x.is_valid() || unpunch_curve_x.is_valid()) { resize_x = true; }
    else { resize_x = false; }

    if (punch_curve_y.is_valid() || unpunch_curve_y.is_valid()) { resize_y = true; }
    else { resize_y = false; }
}
void PunchyContainer::resize_child_by(const float amm)
{
    if (get_child_count() < 1) return;
    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree()) return;
    if (c->is_set_as_top_level()) return;

    Vector2 size = get_size();
    Vector2 child_size = size;
    child_size.x = resize_x ? size.x * punch_curve_x->sample(amm) : size.x;
    child_size.y = resize_y ? size.y * punch_curve_y->sample(amm) : size.y;

    Point2 ofs = (*_resize_child_by)(size, child_size);

    fit_child_in_rect(c, Rect2(ofs, child_size));
}
void PunchyContainer::unresize_child_by(const float amm)
{
    if (get_child_count() < 1) return;
    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree()) return;
    if (c->is_set_as_top_level()) return;

    Vector2 size = get_size();
    Vector2 child_size = size;
    child_size.x = resize_x ? size.x * unpunch_curve_x->sample(amm) : size.x;
    child_size.y = resize_y ? size.y * unpunch_curve_y->sample(amm) : size.y;

    Point2 ofs = (*_resize_child_by)(size, child_size);

    fit_child_in_rect(c, Rect2(ofs, child_size));
}
Point2 PunchyContainer::offset_from_center(const Vector2& size, const Vector2& child_size)
{
    return ((size - child_size) / 2.0).floor();
}
Point2 PunchyContainer::offset_from_left(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.y = int((size.y - child_size.y) / 2.0);
    return ofs;
}
Point2 PunchyContainer::offset_from_right(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.x = int(size.x - child_size.x);
    ofs.y = int((size.y - child_size.y) / 2.0);
    return ofs;
}
Point2 PunchyContainer::offset_from_top(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.x = int((size.x - child_size.x) / 2.0);
    return ofs;
}
Point2 PunchyContainer::offset_from_bottom(const Vector2& size, const Vector2& child_size)
{
    Point2 ofs = Point2();
    ofs.y = int(size.y - child_size.y);
    ofs.x = int((size.x - child_size.x) / 2.0);
    return ofs;
}
void PunchyContainer::emit_punch_finished()
{
    emit_signal("punch_finished", this);
}
void PunchyContainer::emit_unpunch_finished()
{
    emit_signal("unpunch_finished",this);
}
void PunchyContainer::punch()
{
    if (enabled)
        pop_box();
}
void PunchyContainer::unpunch()
{
    if (enabled)
        unpop_box();
}
void PunchyContainer::pop_box()
{
    Ref<Tween> pop_tw = this->create_tween();
    pop_tw->tween_method(Callable(this, "resize_child_by"), 0.0f, 1.0f, punch_duration);
    pop_tw->tween_callback(Callable(this, "emit_punch_finished"));
}
void PunchyContainer::unpop_box()
{
    Ref<Tween> unpop_tw = this->create_tween();
    unpop_tw->tween_method(Callable(this, "unresize_child_by"), 1.0f, 0.0f, unpunch_duration);
    unpop_tw->tween_callback(Callable(this, "emit_unpunch_finished"));
}
bool PunchyContainer::is_mouse_entered() const { return mouse_entered; }
void PunchyContainer::set_enabled(const bool enb) { enabled = enb; }
bool PunchyContainer::get_enabled() const { return enabled; }

void PunchyContainer::set_punch_duration(const float dur) { punch_duration = dur; }
float PunchyContainer::get_punch_duration() const { return punch_duration; }
void PunchyContainer::set_unpunch_duration(const float dur) { unpunch_duration = dur; }
float PunchyContainer::get_unpunch_duration() const { return unpunch_duration; }

Ref<Curve> PunchyContainer::get_punch_curve_x() const { return punch_curve_x; }
void PunchyContainer::set_punch_curve_x(const Ref<Curve> cve)
{
    punch_curve_x = cve;
    if (punch_curve_x.is_valid() || unpunch_curve_x.is_valid()) { resize_x = true; }
    else { resize_x = false; }

    if (!punch_curve_x.is_valid() && unpunch_curve_x.is_valid())
        punch_curve_x.reference_ptr(unpunch_curve_x.ptr());
    if (!unpunch_curve_x.is_valid() && punch_curve_x.is_valid())
        unpunch_curve_x.reference_ptr(punch_curve_x.ptr());
    
    resize_child_by(0.0f);
}
Ref<Curve> PunchyContainer::get_unpunch_curve_x() const { return unpunch_curve_x; }
void PunchyContainer::set_unpunch_curve_x(const Ref<Curve> cve)
{
    unpunch_curve_x = cve;
    if (punch_curve_x.is_valid() || unpunch_curve_x.is_valid()) { resize_x = true; }
    else { resize_x = false; }

    if (!punch_curve_x.is_valid() && unpunch_curve_x.is_valid())
        punch_curve_x.reference_ptr(unpunch_curve_x.ptr());
    if (!unpunch_curve_x.is_valid() && punch_curve_x.is_valid())
        unpunch_curve_x.reference_ptr(punch_curve_x.ptr());
    resize_child_by(0.0f);
}
Ref<Curve> PunchyContainer::get_punch_curve_y() const { return punch_curve_y; }
void PunchyContainer::set_punch_curve_y(const Ref<Curve> cve)
{
    punch_curve_y = cve;
    if (punch_curve_y.is_valid() || unpunch_curve_y.is_valid()) { resize_y = true; }
    else { resize_y = false; }
    if (!punch_curve_y.is_valid() && unpunch_curve_y.is_valid())
        punch_curve_y.reference_ptr(unpunch_curve_y.ptr());
    if (!unpunch_curve_y.is_valid() && punch_curve_y.is_valid())
        unpunch_curve_y.reference_ptr(punch_curve_y.ptr());

    resize_child_by(0.0f);
}
Ref<Curve> PunchyContainer::get_unpunch_curve_y() const { return unpunch_curve_y; }
void PunchyContainer::set_unpunch_curve_y(const Ref<Curve> cve)
{
    unpunch_curve_y = cve;
    if (punch_curve_y.is_valid() || unpunch_curve_y.is_valid()) { resize_y = true; }
    else { resize_y = false; }

    if (!punch_curve_y.is_valid() && unpunch_curve_y.is_valid())
        punch_curve_y.reference_ptr(unpunch_curve_y.ptr());
    if (!unpunch_curve_y.is_valid() && punch_curve_y.is_valid())
        unpunch_curve_y.reference_ptr(punch_curve_y.ptr());
    resize_child_by(0.0f);
}

void PunchyContainer::set_punch_origin(uint32_t str) {
    punch_origin = static_cast<PunchOrigin>(str);
    reset_function();
    resize_child_by(0.0f);
}
uint32_t PunchyContainer::get_punch_origin() const { return punch_origin; }

