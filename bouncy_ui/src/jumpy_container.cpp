#include "jumpy_container.hpp"

JumpyContainer::JumpyContainer()
{
    set_mouse_filter(MOUSE_FILTER_PASS);
    
}
JumpyContainer::~JumpyContainer()
{
}

void JumpyContainer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_rotation_curve", "rotation_curve"), &JumpyContainer::set_rotation_curve);
    ClassDB::bind_method(D_METHOD("get_rotation_curve"), &JumpyContainer::get_rotation_curve);
    ClassDB::bind_method(D_METHOD("set_position_curve_x", "position_curve_x"), &JumpyContainer::set_position_curve_x);
    ClassDB::bind_method(D_METHOD("get_position_curve_x"), &JumpyContainer::get_position_curve_x);
    ClassDB::bind_method(D_METHOD("set_position_curve_y", "position_curve_y"), &JumpyContainer::set_position_curve_y);
    ClassDB::bind_method(D_METHOD("get_position_curve_y"), &JumpyContainer::get_position_curve_y);
    ClassDB::bind_method(D_METHOD("set_scale_curve_x", "scale_curve_x"), &JumpyContainer::set_scale_curve_x);
    ClassDB::bind_method(D_METHOD("get_scale_curve_x"), &JumpyContainer::get_scale_curve_x);
    ClassDB::bind_method(D_METHOD("set_scale_curve_y", "scale_curve_y"), &JumpyContainer::set_scale_curve_y);
    ClassDB::bind_method(D_METHOD("get_scale_curve_y"), &JumpyContainer::get_scale_curve_y);
    ClassDB::bind_method(D_METHOD("set_resting_size","resting_size"), &JumpyContainer::set_resting_size);
    ClassDB::bind_method(D_METHOD("get_resting_size"), &JumpyContainer::get_resting_size);

    ClassDB::bind_method(D_METHOD("jump_child"), &JumpyContainer::jump_child);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "resting_size"), "set_resting_size", "get_resting_size");

    ADD_GROUP("Jump Curves", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "rotation_curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_rotation_curve", "get_rotation_curve");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "position_curve_x", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_position_curve_x", "get_position_curve_x");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "position_curve_y", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_position_curve_y", "get_position_curve_y");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "scale_curve_x", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_scale_curve_x", "get_scale_curve_x");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "scale_curve_y", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_scale_curve_y", "get_scale_curve_y");

}
PackedInt32Array JumpyContainer::_get_allowed_size_flags_horizontal() const
{
    PackedInt32Array flags;
    flags.append(SIZE_FILL);
    flags.append(SIZE_SHRINK_BEGIN);
    flags.append(SIZE_SHRINK_CENTER);
    flags.append(SIZE_SHRINK_END);
    return flags;
}
PackedInt32Array JumpyContainer::_get_allowed_size_flags_vertical() const
{
    PackedInt32Array flags;
    flags.append(SIZE_FILL);
    flags.append(SIZE_SHRINK_BEGIN);
    flags.append(SIZE_SHRINK_CENTER);
    flags.append(SIZE_SHRINK_END);
    return flags;
}
Vector2 JumpyContainer::_get_minimum_size() const
{
    if (get_child_count() < 1) return Vector2();

    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree()) return Vector2();
    if (c->is_set_as_top_level()) return Vector2();

    return c->get_combined_minimum_size();
}
void JumpyContainer::_notification(int p_what) {
    switch (p_what) {
    case NOTIFICATION_READY:
    {
        still_inside = false;
    }break;
    case NOTIFICATION_SORT_CHILDREN:
    {
        jump_child(0.0f);
    } break;
    case NOTIFICATION_RESIZED:
    {
        jump_child(0.0f);
    } break;
    case NOTIFICATION_MOUSE_ENTER:
    {
        if(!still_inside)
            jump();
    }break;
    case NOTIFICATION_MOUSE_EXIT:
    {
        
        if( 0.0f<get_local_mouse_position().x && get_local_mouse_position().x < get_size().x
            && 0.0f < get_local_mouse_position().y && get_local_mouse_position().y < get_size().y)
        {
            still_inside = true;
        }else
        {
            still_inside = false;
        }
    }break;
    }
}
void JumpyContainer::jump()
{
    Ref<Tween> jump_tw = this->create_tween();
    jump_tw->tween_method(Callable(this, "jump_child"), 0.0f, 1.0f, 0.2f);
}
void JumpyContainer::jump_child(double amm)
{
    if (get_child_count() < 1) return ;
    Control* c = Object::cast_to<Control>(get_child(0));
    if (!c || !c->is_visible_in_tree() || 
        c->is_set_as_top_level()) return;

    Vector2 size = get_size();
    Vector2 child_size = size * resting_size;

    child_size.x = scale_curve_x.is_valid() ? child_size.x * scale_curve_x->sample(amm) : child_size.x;
    child_size.y = scale_curve_y.is_valid() ? child_size.y * scale_curve_y->sample(amm) : child_size.y;
    float offset_x = position_curve_x.is_valid() ? -size.x * position_curve_x->sample(amm) : 0.0f ;
    float offset_y = position_curve_y.is_valid() ? -size.y * position_curve_y->sample(amm) : 0.0f ;
    
    Point2 ofs = ((size - child_size) * 0.5f).floor() + Point2(offset_x,offset_y);
    
    fit_child_in_rect(c, Rect2(ofs, child_size));
    if(rotation_curve.is_valid())
        c->set_rotation(-Math::deg_to_rad(rotation_curve->sample(amm)));

}
Ref<Curve> JumpyContainer::get_rotation_curve() const { return rotation_curve; }
void JumpyContainer::set_rotation_curve(const Ref<Curve> cve)
{
    rotation_curve = cve;
    jump_child(0.0f);
}
Ref<Curve> JumpyContainer::get_position_curve_y() const { return position_curve_y; }
void JumpyContainer::set_position_curve_y(const Ref<Curve> cve)
{
    position_curve_y = cve;
    jump_child(0.0f);
}
Ref<Curve> JumpyContainer::get_position_curve_x() const { return position_curve_x; }
void JumpyContainer::set_position_curve_x(const Ref<Curve> cve)
{
    position_curve_x = cve;
    jump_child(0.0f);
}
Ref<Curve> JumpyContainer::get_scale_curve_x() const { return scale_curve_x; }
void JumpyContainer::set_scale_curve_x(const Ref<Curve> cve)
{
    scale_curve_x = cve;
    jump_child(0.0f);
}
Ref<Curve> JumpyContainer::get_scale_curve_y() const { return scale_curve_y; }
void JumpyContainer::set_scale_curve_y(const Ref<Curve> cve)
{
    scale_curve_y = cve;
    jump_child(0.0f);
}
float JumpyContainer::get_resting_size() const { return resting_size; }
void JumpyContainer::set_resting_size(const float size)
{
    resting_size = size;
    jump_child(0.0f);
}