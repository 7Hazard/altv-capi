
/**
 * THIS HEADER IS AUTOGENERATED
 * Jul  8 2019 01:34:50
 * 
 * Source implementation for alt:V C API
 */

#include "cpp-sdk/SDK.h"

#ifdef CAPI_DLL
#ifdef _WIN32
#define CAPI CAPIEXTERN __declspec(dllexport)
#endif // _WIN32
#else
#define CAPI CAPIEXTERN
#endif // CAPI_DLL
#include "altv-capi.hpp"

using namespace alt;

CAPI void alt_VectorLayoutfloat4_free(struct alt_VectorLayoutfloat4* ptr)
{
    delete (class alt::VectorLayout<float, 4> *)ptr;
}

CAPI struct alt_VectorLayoutfloat4* alt_VectorLayoutfloat4_Create()
{
    return (struct alt_VectorLayoutfloat4*)new alt::VectorLayout<float, 4>();
}

CAPI struct alt_VectorLayoutfloat4* alt_VectorLayoutfloat4_Create_1(float** _elements)
{
    return (struct alt_VectorLayoutfloat4*)new alt::VectorLayout<float, 4>((float const (&)[4])_elements);
}

CAPI struct alt_VectorLayoutfloat4* alt_VectorLayoutfloat4_Create_2(float* el)
{
    return (struct alt_VectorLayoutfloat4*)new alt::VectorLayout<float, 4>((const float &)el);
}

CAPI float* alt_VectorLayoutfloat4_Access_size_t(struct alt_VectorLayoutfloat4* _instance, unsigned long long key)
{
    return (float*)&((alt::VectorLayout<float, 4> *)_instance)->operator[]((unsigned long long)key);
}

CAPI float* alt_VectorLayoutfloat4_Access_size_t_1(struct alt_VectorLayoutfloat4* _instance, unsigned long long key)
{
    return (float*)&((alt::VectorLayout<float, 4> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_VectorLayoutfloat4* alt_VectorLayoutfloat4_Create_3(struct alt_VectorLayoutfloat4* _p0)
{
    return (struct alt_VectorLayoutfloat4*)new alt::VectorLayout<float, 4>((const class alt::VectorLayout<float, 4> &)_p0);
}

CAPI struct alt_VectorLayoutfloat4* alt_VectorLayoutfloat4_Create_4(struct alt_VectorLayoutfloat4* _p0)
{
    return (struct alt_VectorLayoutfloat4*)new alt::VectorLayout<float, 4>((class alt::VectorLayout<float, 4> &&)_p0);
}

CAPI void alt_VectorLayoutfloat4_Assign_constVectorLayoutfloat4Ref(struct alt_VectorLayoutfloat4* _instance, struct alt_VectorLayoutfloat4* _p0)
{
    ((alt::VectorLayout<float, 4> *)_instance)->operator=((const class alt::VectorLayout<float, 4> &)_p0);
}

CAPI void alt_VectorLayoutfloat4_Assign_VectorLayoutfloat4RefRef(struct alt_VectorLayoutfloat4* _instance, struct alt_VectorLayoutfloat4* _p0)
{
    ((alt::VectorLayout<float, 4> *)_instance)->operator=((class alt::VectorLayout<float, 4> &&)_p0);
}

CAPI void alt_Vectorfloat4VectorLayoutfloat4_free(struct alt_Vectorfloat4VectorLayoutfloat4* ptr)
{
    delete (class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > *)ptr;
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_Vectorfloat4VectorLayoutfloat4_Create()
{
    return (struct alt_Vectorfloat4VectorLayoutfloat4*)new alt::Vector<float, 4, alt::VectorLayout<float, 4> >();
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_Vectorfloat4VectorLayoutfloat4_Create_1(float** _elements)
{
    return (struct alt_Vectorfloat4VectorLayoutfloat4*)new alt::Vector<float, 4, alt::VectorLayout<float, 4> >((float const (&)[4])_elements);
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_Vectorfloat4VectorLayoutfloat4_Create_2(struct alt_VectorLayoutfloat4* layout)
{
    return (struct alt_Vectorfloat4VectorLayoutfloat4*)new alt::Vector<float, 4, alt::VectorLayout<float, 4> >((const class alt::VectorLayout<float, 4> &)layout);
}

CAPI float alt_Vectorfloat4VectorLayoutfloat4_LengthSqr(struct alt_Vectorfloat4VectorLayoutfloat4* _instance)
{
    return (float)((alt::Vector<float, 4, alt::VectorLayout<float, 4> > *)_instance)->LengthSqr();
}

CAPI float alt_Vectorfloat4VectorLayoutfloat4_Length(struct alt_Vectorfloat4VectorLayoutfloat4* _instance)
{
    return (float)((alt::Vector<float, 4, alt::VectorLayout<float, 4> > *)_instance)->Length();
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_Vectorfloat4VectorLayoutfloat4_Create_3(struct alt_Vectorfloat4VectorLayoutfloat4* _p0)
{
    return (struct alt_Vectorfloat4VectorLayoutfloat4*)new alt::Vector<float, 4, alt::VectorLayout<float, 4> >((const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)_p0);
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_Vectorfloat4VectorLayoutfloat4_Create_4(struct alt_Vectorfloat4VectorLayoutfloat4* _p0)
{
    return (struct alt_Vectorfloat4VectorLayoutfloat4*)new alt::Vector<float, 4, alt::VectorLayout<float, 4> >((class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &&)_p0);
}

CAPI void alt_Vectorfloat4VectorLayoutfloat4_Assign_constVectorfloat4VectorLayoutfloat4Ref(struct alt_Vectorfloat4VectorLayoutfloat4* _instance, struct alt_Vectorfloat4VectorLayoutfloat4* _p0)
{
    ((alt::Vector<float, 4, alt::VectorLayout<float, 4> > *)_instance)->operator=((const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)_p0);
}

CAPI void alt_Vectorfloat4VectorLayoutfloat4_Assign_Vectorfloat4VectorLayoutfloat4RefRef(struct alt_Vectorfloat4VectorLayoutfloat4* _instance, struct alt_Vectorfloat4VectorLayoutfloat4* _p0)
{
    ((alt::Vector<float, 4, alt::VectorLayout<float, 4> > *)_instance)->operator=((class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &&)_p0);
}

CAPI void alt_PointLayout_free(struct alt_PointLayout* ptr)
{
    delete (class alt::PointLayout *)ptr;
}

CAPI struct alt_PointLayout* alt_PointLayout_Create()
{
    return (struct alt_PointLayout*)new alt::PointLayout();
}

CAPI struct alt_PointLayout* alt_PointLayout_Create_1(float _x, float _y, float _z)
{
    return (struct alt_PointLayout*)new alt::PointLayout((float)_x, (float)_y, (float)_z);
}

CAPI struct alt_PointLayout* alt_PointLayout_Create_2(float** elements)
{
    return (struct alt_PointLayout*)new alt::PointLayout((float const (&)[3])elements);
}

CAPI float* alt_PointLayout_Access_size_t(struct alt_PointLayout* _instance, unsigned long long key)
{
    return (float*)&((alt::PointLayout *)_instance)->operator[]((unsigned long long)key);
}

CAPI float* alt_PointLayout_Access_size_t_1(struct alt_PointLayout* _instance, unsigned long long key)
{
    return (float*)&((alt::PointLayout *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_PointLayout* alt_PointLayout_Create_3(struct alt_PointLayout* _p0)
{
    return (struct alt_PointLayout*)new alt::PointLayout((const class alt::PointLayout &)_p0);
}

CAPI struct alt_PointLayout* alt_PointLayout_Create_4(struct alt_PointLayout* _p0)
{
    return (struct alt_PointLayout*)new alt::PointLayout((class alt::PointLayout &&)_p0);
}

CAPI void alt_PointPaddedLayout_free(struct alt_PointPaddedLayout* ptr)
{
    delete (class alt::PointPaddedLayout *)ptr;
}

CAPI struct alt_PointPaddedLayout* alt_PointPaddedLayout_Create()
{
    return (struct alt_PointPaddedLayout*)new alt::PointPaddedLayout();
}

CAPI struct alt_PointPaddedLayout* alt_PointPaddedLayout_Create_1(float x, float y, float z)
{
    return (struct alt_PointPaddedLayout*)new alt::PointPaddedLayout((float)x, (float)y, (float)z);
}

CAPI struct alt_PointPaddedLayout* alt_PointPaddedLayout_Create_2(float** elements)
{
    return (struct alt_PointPaddedLayout*)new alt::PointPaddedLayout((float const (&)[3])elements);
}

CAPI void alt_RotationLayout_free(struct alt_RotationLayout* ptr)
{
    delete (class alt::RotationLayout *)ptr;
}

CAPI struct alt_RotationLayout* alt_RotationLayout_Create(float _roll, float _pitch, float _yaw)
{
    return (struct alt_RotationLayout*)new alt::RotationLayout((float)_roll, (float)_pitch, (float)_yaw);
}

CAPI struct alt_RotationLayout* alt_RotationLayout_Create_1()
{
    return (struct alt_RotationLayout*)new alt::RotationLayout();
}

CAPI struct alt_RotationLayout* alt_RotationLayout_Create_2(float** elements)
{
    return (struct alt_RotationLayout*)new alt::RotationLayout((float const (&)[3])elements);
}

CAPI float* alt_RotationLayout_Access_size_t(struct alt_RotationLayout* _instance, unsigned long long key)
{
    return (float*)&((alt::RotationLayout *)_instance)->operator[]((unsigned long long)key);
}

CAPI float* alt_RotationLayout_Access_size_t_1(struct alt_RotationLayout* _instance, unsigned long long key)
{
    return (float*)&((alt::RotationLayout *)_instance)->operator[]((unsigned long long)key);
}

CAPI void alt_RotationLayout_Normalize(struct alt_RotationLayout* _instance)
{
    (void)((alt::RotationLayout *)_instance)->Normalize();
}

CAPI struct alt_RotationLayout* alt_RotationLayout_Create_3(struct alt_RotationLayout* _p0)
{
    return (struct alt_RotationLayout*)new alt::RotationLayout((const class alt::RotationLayout &)_p0);
}

CAPI struct alt_RotationLayout* alt_RotationLayout_Create_4(struct alt_RotationLayout* _p0)
{
    return (struct alt_RotationLayout*)new alt::RotationLayout((class alt::RotationLayout &&)_p0);
}

CAPI void alt_RotationPaddedLayout_free(struct alt_RotationPaddedLayout* ptr)
{
    delete (class alt::RotationPaddedLayout *)ptr;
}

CAPI struct alt_RotationPaddedLayout* alt_RotationPaddedLayout_Create()
{
    return (struct alt_RotationPaddedLayout*)new alt::RotationPaddedLayout();
}

CAPI struct alt_RotationPaddedLayout* alt_RotationPaddedLayout_Create_1(float roll, float pitch, float yaw)
{
    return (struct alt_RotationPaddedLayout*)new alt::RotationPaddedLayout((float)roll, (float)pitch, (float)yaw);
}

CAPI struct alt_RotationPaddedLayout* alt_RotationPaddedLayout_Create_2(float** elements)
{
    return (struct alt_RotationPaddedLayout*)new alt::RotationPaddedLayout((float const (&)[3])elements);
}

CAPI void alt_ViewMatrixLayout_free(struct alt_ViewMatrixLayout* ptr)
{
    delete (class alt::ViewMatrixLayout *)ptr;
}

CAPI struct alt_ViewMatrixLayout* alt_ViewMatrixLayout_Create()
{
    return (struct alt_ViewMatrixLayout*)new alt::ViewMatrixLayout();
}

CAPI struct alt_ViewMatrixLayout* alt_ViewMatrixLayout_Create_1(struct alt_Vectorfloat4VectorLayoutfloat4** _rows)
{
    return (struct alt_ViewMatrixLayout*)new alt::ViewMatrixLayout((class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > const (&)[4])_rows);
}

CAPI struct alt_ViewMatrixLayout* alt_ViewMatrixLayout_Create_2(struct alt_Vectorfloat4VectorLayoutfloat4* row)
{
    return (struct alt_ViewMatrixLayout*)new alt::ViewMatrixLayout((const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)row);
}

CAPI struct alt_ViewMatrixLayout* alt_ViewMatrixLayout_Create_3(float** elements)
{
    return (struct alt_ViewMatrixLayout*)new alt::ViewMatrixLayout((float const (&)[16])elements);
}

CAPI struct alt_ViewMatrixLayout* alt_ViewMatrixLayout_Create_4(struct alt_Vectorfloat4VectorLayoutfloat4* _right, struct alt_Vectorfloat4VectorLayoutfloat4* _up, struct alt_Vectorfloat4VectorLayoutfloat4* _forward, struct alt_Vectorfloat4VectorLayoutfloat4* _pos)
{
    return (struct alt_ViewMatrixLayout*)new alt::ViewMatrixLayout((const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)_right, (const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)_up, (const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)_forward, (const class alt::Vector<float, 4, class alt::VectorLayout<float, 4> > &)_pos);
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_ViewMatrixLayout_Access_size_t(struct alt_ViewMatrixLayout* _instance, unsigned long long key)
{
    return (alt_Vectorfloat4VectorLayoutfloat4*)&((alt::ViewMatrixLayout *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_Vectorfloat4VectorLayoutfloat4* alt_ViewMatrixLayout_Access_size_t_1(struct alt_ViewMatrixLayout* _instance, unsigned long long key)
{
    return (alt_Vectorfloat4VectorLayoutfloat4*)&((alt::ViewMatrixLayout *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_String* alt_String_Create(unsigned long long _size, char fill)
{
    return (struct alt_String*)new alt::String((unsigned long long)_size, (char)fill);
}

CAPI struct alt_String* alt_String_Create_1(char* _data, unsigned long long size)
{
    return (struct alt_String*)new alt::String((const char *)_data, (unsigned long long)size);
}

CAPI struct alt_String* alt_String_Create_2()
{
    return (struct alt_String*)new alt::String();
}

CAPI struct alt_String* alt_String_Create_3(char* _str)
{
    return (struct alt_String*)new alt::String((const char *)_str);
}

CAPI struct alt_String* alt_String_Create_5(struct alt_String* that)
{
    return (struct alt_String*)new alt::String((const class alt::String &)that);
}

CAPI struct alt_String* alt_String_Create_6(struct alt_String* that)
{
    return (struct alt_String*)new alt::String((class alt::String &&)that);
}

CAPI char* alt_String_Access_uint64_t(struct alt_String* _instance, unsigned long long key)
{
    return (char*)&((alt::String *)_instance)->operator[]((unsigned long long)key);
}

CAPI char alt_String_Access_uint64_t_1(struct alt_String* _instance, unsigned long long key)
{
    return (char)((alt::String *)_instance)->operator[]((unsigned long long)key);
}

CAPI void alt_String_Assign_constStringRef(struct alt_String* _instance, struct alt_String* that)
{
    ((alt::String *)_instance)->operator=((const class alt::String &)that);
}

CAPI void alt_String_Assign_StringRefRef(struct alt_String* _instance, struct alt_String* that)
{
    ((alt::String *)_instance)->operator=((class alt::String &&)that);
}

CAPI _Bool alt_String_IsEmpty(struct alt_String* _instance)
{
    return (_Bool)((alt::String *)_instance)->IsEmpty();
}

CAPI char* alt_String_GetData(struct alt_String* _instance)
{
    return (char*)((alt::String *)_instance)->GetData();
}

CAPI char* alt_String_GetData_1(struct alt_String* _instance)
{
    return (char*)((alt::String *)_instance)->GetData();
}

CAPI unsigned long long alt_String_GetSize(struct alt_String* _instance)
{
    return (unsigned long long)((alt::String *)_instance)->GetSize();
}

CAPI _Bool alt_String_Equals_String(struct alt_String* _instance, struct alt_String* that)
{
    return (_Bool)((alt::String *)_instance)->operator==(*(class alt::String*)that);
}

CAPI _Bool alt_String_NotEquals_String(struct alt_String* _instance, struct alt_String* that)
{
    return (_Bool)((alt::String *)_instance)->operator!=(*(class alt::String*)that);
}

CAPI char* alt_String_CStr(struct alt_String* _instance)
{
    return (char*)((alt::String *)_instance)->CStr();
}

CAPI void alt_StringView_free(struct alt_StringView* ptr)
{
    delete (class alt::StringView *)ptr;
}

CAPI struct alt_StringView* alt_StringView_Create(char* _data, unsigned long long _size)
{
    return (struct alt_StringView*)new alt::StringView((const char *)_data, (unsigned long long)_size);
}

CAPI struct alt_StringView* alt_StringView_Create_1()
{
    return (struct alt_StringView*)new alt::StringView();
}

CAPI struct alt_StringView* alt_StringView_Create_2(char* _str)
{
    return (struct alt_StringView*)new alt::StringView((const char *)_str);
}

CAPI struct alt_StringView* alt_StringView_Create_4(struct alt_String* _str)
{
    return (struct alt_StringView*)new alt::StringView((const class alt::String &)_str);
}

CAPI _Bool alt_StringView_IsEmpty(struct alt_StringView* _instance)
{
    return (_Bool)((alt::StringView *)_instance)->IsEmpty();
}

CAPI char* alt_StringView_GetData(struct alt_StringView* _instance)
{
    return (char*)((alt::StringView *)_instance)->GetData();
}

CAPI unsigned long long alt_StringView_GetSize(struct alt_StringView* _instance)
{
    return (unsigned long long)((alt::StringView *)_instance)->GetSize();
}

CAPI char alt_StringView_Access_uint64_t(struct alt_StringView* _instance, unsigned long long key)
{
    return (char)((alt::StringView *)_instance)->operator[]((unsigned long long)key);
}

CAPI _Bool alt_StringView_Equals_StringView(struct alt_StringView* _instance, struct alt_StringView* that)
{
    return (_Bool)((alt::StringView *)_instance)->operator==(*(class alt::StringView*)that);
}

CAPI _Bool alt_StringView_NotEquals_StringView(struct alt_StringView* _instance, struct alt_StringView* that)
{
    return (_Bool)((alt::StringView *)_instance)->operator!=(*(class alt::StringView*)that);
}

CAPI struct alt_String* alt_StringView_To_String(struct alt_StringView* _instance)
{
    return (alt_String*)new class alt::String(((alt::StringView *)_instance)->operator String());
}

CAPI char* alt_StringView_CStr(struct alt_StringView* _instance)
{
    return (char*)((alt::StringView *)_instance)->CStr();
}

CAPI struct alt_StringView* alt_StringView_Create_6(struct alt_StringView* _p0)
{
    return (struct alt_StringView*)new alt::StringView((const class alt::StringView &)_p0);
}

CAPI struct alt_StringView* alt_StringView_Create_7(struct alt_StringView* _p0)
{
    return (struct alt_StringView*)new alt::StringView((class alt::StringView &&)_p0);
}

CAPI void alt_StringView_Assign_constStringViewRef(struct alt_StringView* _instance, struct alt_StringView* _p0)
{
    ((alt::StringView *)_instance)->operator=((const class alt::StringView &)_p0);
}

CAPI void alt_StringView_Assign_StringViewRefRef(struct alt_StringView* _instance, struct alt_StringView* _p0)
{
    ((alt::StringView *)_instance)->operator=((class alt::StringView &&)_p0);
}

CAPI void alt_RGBA_free(struct alt_RGBA* ptr)
{
    delete (struct alt::RGBA *)ptr;
}

CAPI struct alt_RGBA* alt_RGBA_Create()
{
    return (struct alt_RGBA*)new alt::RGBA();
}

CAPI struct alt_RGBA* alt_RGBA_Create_1(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
    return (struct alt_RGBA*)new alt::RGBA((unsigned char)_r, (unsigned char)_g, (unsigned char)_b, (unsigned char)_a);
}

CAPI struct alt_ArrayMValue* alt_ArrayMValue_Create()
{
    return (struct alt_ArrayMValue*)new alt::Array<alt::MValue>();
}

CAPI struct alt_ArrayMValue* alt_ArrayMValue_Create_1(struct alt_MValue* _data, unsigned long long _size)
{
    return (struct alt_ArrayMValue*)new alt::Array<alt::MValue>((const class alt::MValue *)_data, (unsigned long long)_size);
}

CAPI struct alt_ArrayMValue* alt_ArrayMValue_Create_2(unsigned long long _size, struct alt_MValue* fill)
{
    return (struct alt_ArrayMValue*)new alt::Array<alt::MValue>((unsigned long long)_size, (const class alt::MValue &)fill);
}

CAPI struct alt_ArrayMValue* alt_ArrayMValue_Create_3(struct alt_ArrayMValue* that)
{
    return (struct alt_ArrayMValue*)new alt::Array<alt::MValue>((const class alt::Array<class alt::MValue> &)that);
}

CAPI struct alt_ArrayMValue* alt_ArrayMValue_Create_4(struct alt_ArrayMValue* that)
{
    return (struct alt_ArrayMValue*)new alt::Array<alt::MValue>((class alt::Array<class alt::MValue> &&)that);
}

CAPI void alt_ArrayMValue_Assign_constArrayMValueRef(struct alt_ArrayMValue* _instance, struct alt_ArrayMValue* that)
{
    ((alt::Array<alt::MValue> *)_instance)->operator=((const class alt::Array<class alt::MValue> &)that);
}

CAPI void alt_ArrayMValue_Assign_ArrayMValueRefRef(struct alt_ArrayMValue* _instance, struct alt_ArrayMValue* that)
{
    ((alt::Array<alt::MValue> *)_instance)->operator=((class alt::Array<class alt::MValue> &&)that);
}

CAPI void alt_ArrayMValue_Push(struct alt_ArrayMValue* _instance, struct alt_MValue* el)
{
    (void)((alt::Array<alt::MValue> *)_instance)->Push((const class alt::MValue &)el);
}

CAPI unsigned long long alt_ArrayMValue_GetSize(struct alt_ArrayMValue* _instance)
{
    return (unsigned long long)((alt::Array<alt::MValue> *)_instance)->GetSize();
}

CAPI unsigned long long alt_ArrayMValue_GetCapacity(struct alt_ArrayMValue* _instance)
{
    return (unsigned long long)((alt::Array<alt::MValue> *)_instance)->GetCapacity();
}

CAPI struct alt_MValue* alt_ArrayMValue_Access_uint64_t(struct alt_ArrayMValue* _instance, unsigned long long key)
{
    return (alt_MValue*)&((alt::Array<alt::MValue> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_MValue* alt_ArrayMValue_Access_uint64_t_1(struct alt_ArrayMValue* _instance, unsigned long long key)
{
    return (alt_MValue*)&((alt::Array<alt::MValue> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_MValue* alt_ArrayMValue_begin(struct alt_ArrayMValue* _instance)
{
    return (alt_MValue*)((alt::Array<alt::MValue> *)_instance)->begin();
}

CAPI struct alt_MValue* alt_ArrayMValue_begin_1(struct alt_ArrayMValue* _instance)
{
    return (alt_MValue*)((alt::Array<alt::MValue> *)_instance)->begin();
}

CAPI struct alt_MValue* alt_ArrayMValue_end(struct alt_ArrayMValue* _instance)
{
    return (alt_MValue*)((alt::Array<alt::MValue> *)_instance)->end();
}

CAPI struct alt_MValue* alt_ArrayMValue_end_1(struct alt_ArrayMValue* _instance)
{
    return (alt_MValue*)((alt::Array<alt::MValue> *)_instance)->end();
}

CAPI struct alt_ArrayString* alt_ArrayString_Create()
{
    return (struct alt_ArrayString*)new alt::Array<alt::String>();
}

CAPI struct alt_ArrayString* alt_ArrayString_Create_1(struct alt_String* _data, unsigned long long _size)
{
    return (struct alt_ArrayString*)new alt::Array<alt::String>((const class alt::String *)_data, (unsigned long long)_size);
}

CAPI struct alt_ArrayString* alt_ArrayString_Create_2(unsigned long long _size, struct alt_String* fill)
{
    return (struct alt_ArrayString*)new alt::Array<alt::String>((unsigned long long)_size, (const class alt::String &)fill);
}

CAPI struct alt_ArrayString* alt_ArrayString_Create_3(struct alt_ArrayString* that)
{
    return (struct alt_ArrayString*)new alt::Array<alt::String>((const class alt::Array<class alt::String> &)that);
}

CAPI struct alt_ArrayString* alt_ArrayString_Create_4(struct alt_ArrayString* that)
{
    return (struct alt_ArrayString*)new alt::Array<alt::String>((class alt::Array<class alt::String> &&)that);
}

CAPI void alt_ArrayString_Assign_constArrayStringRef(struct alt_ArrayString* _instance, struct alt_ArrayString* that)
{
    ((alt::Array<alt::String> *)_instance)->operator=((const class alt::Array<class alt::String> &)that);
}

CAPI void alt_ArrayString_Assign_ArrayStringRefRef(struct alt_ArrayString* _instance, struct alt_ArrayString* that)
{
    ((alt::Array<alt::String> *)_instance)->operator=((class alt::Array<class alt::String> &&)that);
}

CAPI void alt_ArrayString_Push(struct alt_ArrayString* _instance, struct alt_String* el)
{
    (void)((alt::Array<alt::String> *)_instance)->Push((const class alt::String &)el);
}

CAPI unsigned long long alt_ArrayString_GetSize(struct alt_ArrayString* _instance)
{
    return (unsigned long long)((alt::Array<alt::String> *)_instance)->GetSize();
}

CAPI unsigned long long alt_ArrayString_GetCapacity(struct alt_ArrayString* _instance)
{
    return (unsigned long long)((alt::Array<alt::String> *)_instance)->GetCapacity();
}

CAPI struct alt_String* alt_ArrayString_Access_uint64_t(struct alt_ArrayString* _instance, unsigned long long key)
{
    return (alt_String*)&((alt::Array<alt::String> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_String* alt_ArrayString_Access_uint64_t_1(struct alt_ArrayString* _instance, unsigned long long key)
{
    return (alt_String*)&((alt::Array<alt::String> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_String* alt_ArrayString_begin(struct alt_ArrayString* _instance)
{
    return (alt_String*)((alt::Array<alt::String> *)_instance)->begin();
}

CAPI struct alt_String* alt_ArrayString_begin_1(struct alt_ArrayString* _instance)
{
    return (alt_String*)((alt::Array<alt::String> *)_instance)->begin();
}

CAPI struct alt_String* alt_ArrayString_end(struct alt_ArrayString* _instance)
{
    return (alt_String*)((alt::Array<alt::String> *)_instance)->end();
}

CAPI struct alt_String* alt_ArrayString_end_1(struct alt_ArrayString* _instance)
{
    return (alt_String*)((alt::Array<alt::String> *)_instance)->end();
}

CAPI struct alt_ArrayStringView* alt_ArrayStringView_Create()
{
    return (struct alt_ArrayStringView*)new alt::Array<alt::StringView>();
}

CAPI struct alt_ArrayStringView* alt_ArrayStringView_Create_1(struct alt_StringView* _data, unsigned long long _size)
{
    return (struct alt_ArrayStringView*)new alt::Array<alt::StringView>((const class alt::StringView *)_data, (unsigned long long)_size);
}

CAPI struct alt_ArrayStringView* alt_ArrayStringView_Create_2(unsigned long long _size, struct alt_StringView* fill)
{
    return (struct alt_ArrayStringView*)new alt::Array<alt::StringView>((unsigned long long)_size, (const class alt::StringView &)fill);
}

CAPI struct alt_ArrayStringView* alt_ArrayStringView_Create_3(struct alt_ArrayStringView* that)
{
    return (struct alt_ArrayStringView*)new alt::Array<alt::StringView>((const class alt::Array<class alt::StringView> &)that);
}

CAPI struct alt_ArrayStringView* alt_ArrayStringView_Create_4(struct alt_ArrayStringView* that)
{
    return (struct alt_ArrayStringView*)new alt::Array<alt::StringView>((class alt::Array<class alt::StringView> &&)that);
}

CAPI void alt_ArrayStringView_Assign_constArrayStringViewRef(struct alt_ArrayStringView* _instance, struct alt_ArrayStringView* that)
{
    ((alt::Array<alt::StringView> *)_instance)->operator=((const class alt::Array<class alt::StringView> &)that);
}

CAPI void alt_ArrayStringView_Assign_ArrayStringViewRefRef(struct alt_ArrayStringView* _instance, struct alt_ArrayStringView* that)
{
    ((alt::Array<alt::StringView> *)_instance)->operator=((class alt::Array<class alt::StringView> &&)that);
}

CAPI void alt_ArrayStringView_Push(struct alt_ArrayStringView* _instance, struct alt_StringView* el)
{
    (void)((alt::Array<alt::StringView> *)_instance)->Push((const class alt::StringView &)el);
}

CAPI unsigned long long alt_ArrayStringView_GetSize(struct alt_ArrayStringView* _instance)
{
    return (unsigned long long)((alt::Array<alt::StringView> *)_instance)->GetSize();
}

CAPI unsigned long long alt_ArrayStringView_GetCapacity(struct alt_ArrayStringView* _instance)
{
    return (unsigned long long)((alt::Array<alt::StringView> *)_instance)->GetCapacity();
}

CAPI struct alt_StringView* alt_ArrayStringView_Access_uint64_t(struct alt_ArrayStringView* _instance, unsigned long long key)
{
    return (alt_StringView*)&((alt::Array<alt::StringView> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_StringView* alt_ArrayStringView_Access_uint64_t_1(struct alt_ArrayStringView* _instance, unsigned long long key)
{
    return (alt_StringView*)&((alt::Array<alt::StringView> *)_instance)->operator[]((unsigned long long)key);
}

CAPI struct alt_StringView* alt_ArrayStringView_begin(struct alt_ArrayStringView* _instance)
{
    return (alt_StringView*)((alt::Array<alt::StringView> *)_instance)->begin();
}

CAPI struct alt_StringView* alt_ArrayStringView_begin_1(struct alt_ArrayStringView* _instance)
{
    return (alt_StringView*)((alt::Array<alt::StringView> *)_instance)->begin();
}

CAPI struct alt_StringView* alt_ArrayStringView_end(struct alt_ArrayStringView* _instance)
{
    return (alt_StringView*)((alt::Array<alt::StringView> *)_instance)->end();
}

CAPI struct alt_StringView* alt_ArrayStringView_end_1(struct alt_ArrayStringView* _instance)
{
    return (alt_StringView*)((alt::Array<alt::StringView> *)_instance)->end();
}

CAPI struct alt_MValue* alt_MValue_Create()
{
    return (struct alt_MValue*)new alt::MValue();
}

CAPI struct alt_MValue* alt_MValue_Create_1(_Bool* val)
{
    return (struct alt_MValue*)new alt::MValue((const _Bool &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_2(long long* val)
{
    return (struct alt_MValue*)new alt::MValue((const long long &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_3(unsigned long long* val)
{
    return (struct alt_MValue*)new alt::MValue((const unsigned long long &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_4(double* val)
{
    return (struct alt_MValue*)new alt::MValue((const double &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_5(struct alt_String* val)
{
    return (struct alt_MValue*)new alt::MValue((const class alt::String &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_6(struct alt_ArrayMValue* val)
{
    return (struct alt_MValue*)new alt::MValue((const class alt::Array<class alt::MValue> &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_8(struct alt_IBaseObject** val)
{
    return (struct alt_MValue*)new alt::MValue((class alt::IBaseObject *const &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_9(struct alt_MValue_Function* val)
{
    return (struct alt_MValue*)new alt::MValue((const struct alt::MValue::Function &)val);
}

CAPI struct alt_MValue* alt_MValue_Create_10(int val)
{
    return (struct alt_MValue*)new alt::MValue((int)val);
}

CAPI struct alt_MValue* alt_MValue_Create_11(unsigned int val)
{
    return (struct alt_MValue*)new alt::MValue((unsigned int)val);
}

CAPI struct alt_MValue* alt_MValue_Create_12(char* val)
{
    return (struct alt_MValue*)new alt::MValue((const char *)val);
}

CAPI struct alt_MValue* alt_MValue_Create_14(struct alt_MValue* that)
{
    return (struct alt_MValue*)new alt::MValue((const class alt::MValue &)that);
}

CAPI void alt_MValue_Assign_constMValueRef(struct alt_MValue* _instance, struct alt_MValue* that)
{
    ((alt::MValue *)_instance)->operator=((const class alt::MValue &)that);
}

CAPI _Bool alt_MValue_ToBool(struct alt_MValue* _instance)
{
    return (_Bool)((alt::MValue *)_instance)->ToBool();
}

CAPI long long alt_MValue_ToInt(struct alt_MValue* _instance)
{
    return (long long)((alt::MValue *)_instance)->ToInt();
}

CAPI unsigned long long alt_MValue_ToUInt(struct alt_MValue* _instance)
{
    return (unsigned long long)((alt::MValue *)_instance)->ToUInt();
}

CAPI struct alt_String* alt_MValue_ToString(struct alt_MValue* _instance)
{
    return (alt_String*)new class alt::String(((alt::MValue *)_instance)->ToString());
}

CAPI enum alt_MValue_Type alt_MValue_GetType(struct alt_MValue* _instance)
{
    return (alt_MValue_Type)((alt::MValue *)_instance)->GetType();
}

CAPI void alt_MValue_Function_free(struct alt_MValue_Function* ptr)
{
    delete (struct alt::MValue::Function *)ptr;
}

CAPI struct alt_MValue_Function* alt_MValue_Function_Create()
{
    return (struct alt_MValue_Function*)new alt::MValue::Function();
}

CAPI struct alt_MValue_Function* alt_MValue_Function_Create_1(struct alt_MValue_Function* _p0)
{
    return (struct alt_MValue_Function*)new alt::MValue::Function((const struct alt::MValue::Function &)_p0);
}

CAPI struct alt_MValue_Function* alt_MValue_Function_Create_2(struct alt_MValue_Function* _p0)
{
    return (struct alt_MValue_Function*)new alt::MValue::Function((struct alt::MValue::Function &&)_p0);
}

CAPI void alt_MValue_Function_Assign_constMValue_FunctionRef(struct alt_MValue_Function* _instance, struct alt_MValue_Function* _p0)
{
    ((alt::MValue::Function *)_instance)->operator=((const struct alt::MValue::Function &)_p0);
}

CAPI void alt_MValue_Function_Assign_MValue_FunctionRefRef(struct alt_MValue_Function* _instance, struct alt_MValue_Function* _p0)
{
    ((alt::MValue::Function *)_instance)->operator=((struct alt::MValue::Function &&)_p0);
}

CAPI void alt_MValue_Storage_Bool_free(struct alt_MValue_Storage_Bool* ptr)
{
    delete (struct alt::MValue::Storage<_Bool> *)ptr;
}

CAPI struct alt_MValue_Storage_Bool* alt_MValue_Storage_Bool_Create(struct alt_MValue_Storage_Bool* _p0)
{
    return (struct alt_MValue_Storage_Bool*)new alt::MValue::Storage<_Bool>((const struct alt::MValue::Storage<_Bool> &)_p0);
}

CAPI struct alt_MValue_Storage_Bool* alt_MValue_Storage_Bool_Create_1(struct alt_MValue_Storage_Bool* _p0)
{
    return (struct alt_MValue_Storage_Bool*)new alt::MValue::Storage<_Bool>((struct alt::MValue::Storage<_Bool> &&)_p0);
}

CAPI void alt_MValue_Storage_Bool_Assign_constMValue_Storage_BoolRef(struct alt_MValue_Storage_Bool* _instance, struct alt_MValue_Storage_Bool* _p0)
{
    ((alt::MValue::Storage<_Bool> *)_instance)->operator=((const struct alt::MValue::Storage<_Bool> &)_p0);
}

CAPI void alt_MValue_Storage_Bool_Assign_MValue_Storage_BoolRefRef(struct alt_MValue_Storage_Bool* _instance, struct alt_MValue_Storage_Bool* _p0)
{
    ((alt::MValue::Storage<_Bool> *)_instance)->operator=((struct alt::MValue::Storage<_Bool> &&)_p0);
}

CAPI struct alt_MValue_Storage_Bool* alt_MValue_Storage_Bool_Create_2()
{
    return (struct alt_MValue_Storage_Bool*)new alt::MValue::Storage<_Bool>();
}

CAPI void alt_MValue_Storagelonglong_free(struct alt_MValue_Storagelonglong* ptr)
{
    delete (struct alt::MValue::Storage<long long> *)ptr;
}

CAPI struct alt_MValue_Storagelonglong* alt_MValue_Storagelonglong_Create(struct alt_MValue_Storagelong_long* _p0)
{
    return (struct alt_MValue_Storagelonglong*)new alt::MValue::Storage<long long>((const struct alt::MValue::Storage<long long> &)_p0);
}

CAPI struct alt_MValue_Storagelonglong* alt_MValue_Storagelonglong_Create_1(struct alt_MValue_Storagelong_long* _p0)
{
    return (struct alt_MValue_Storagelonglong*)new alt::MValue::Storage<long long>((struct alt::MValue::Storage<long long> &&)_p0);
}

CAPI void alt_MValue_Storagelonglong_Assign_constMValue_StoragelonglongRef(struct alt_MValue_Storagelonglong* _instance, struct alt_MValue_Storagelong_long* _p0)
{
    ((alt::MValue::Storage<long long> *)_instance)->operator=((const struct alt::MValue::Storage<long long> &)_p0);
}

CAPI void alt_MValue_Storagelonglong_Assign_MValue_StoragelonglongRefRef(struct alt_MValue_Storagelonglong* _instance, struct alt_MValue_Storagelong_long* _p0)
{
    ((alt::MValue::Storage<long long> *)_instance)->operator=((struct alt::MValue::Storage<long long> &&)_p0);
}

CAPI struct alt_MValue_Storagelonglong* alt_MValue_Storagelonglong_Create_2()
{
    return (struct alt_MValue_Storagelonglong*)new alt::MValue::Storage<long long>();
}

CAPI void alt_MValue_Storageunsignedlonglong_free(struct alt_MValue_Storageunsignedlonglong* ptr)
{
    delete (struct alt::MValue::Storage<unsigned long long> *)ptr;
}

CAPI struct alt_MValue_Storageunsignedlonglong* alt_MValue_Storageunsignedlonglong_Create(struct alt_MValue_Storageunsigned_long_long* _p0)
{
    return (struct alt_MValue_Storageunsignedlonglong*)new alt::MValue::Storage<unsigned long long>((const struct alt::MValue::Storage<unsigned long long> &)_p0);
}

CAPI struct alt_MValue_Storageunsignedlonglong* alt_MValue_Storageunsignedlonglong_Create_1(struct alt_MValue_Storageunsigned_long_long* _p0)
{
    return (struct alt_MValue_Storageunsignedlonglong*)new alt::MValue::Storage<unsigned long long>((struct alt::MValue::Storage<unsigned long long> &&)_p0);
}

CAPI void alt_MValue_Storageunsignedlonglong_Assign_constMValue_StorageunsignedlonglongRef(struct alt_MValue_Storageunsignedlonglong* _instance, struct alt_MValue_Storageunsigned_long_long* _p0)
{
    ((alt::MValue::Storage<unsigned long long> *)_instance)->operator=((const struct alt::MValue::Storage<unsigned long long> &)_p0);
}

CAPI void alt_MValue_Storageunsignedlonglong_Assign_MValue_StorageunsignedlonglongRefRef(struct alt_MValue_Storageunsignedlonglong* _instance, struct alt_MValue_Storageunsigned_long_long* _p0)
{
    ((alt::MValue::Storage<unsigned long long> *)_instance)->operator=((struct alt::MValue::Storage<unsigned long long> &&)_p0);
}

CAPI struct alt_MValue_Storageunsignedlonglong* alt_MValue_Storageunsignedlonglong_Create_2()
{
    return (struct alt_MValue_Storageunsignedlonglong*)new alt::MValue::Storage<unsigned long long>();
}

CAPI void alt_MValue_Storagedouble_free(struct alt_MValue_Storagedouble* ptr)
{
    delete (struct alt::MValue::Storage<double> *)ptr;
}

CAPI struct alt_MValue_Storagedouble* alt_MValue_Storagedouble_Create(struct alt_MValue_Storagedouble* _p0)
{
    return (struct alt_MValue_Storagedouble*)new alt::MValue::Storage<double>((const struct alt::MValue::Storage<double> &)_p0);
}

CAPI struct alt_MValue_Storagedouble* alt_MValue_Storagedouble_Create_1(struct alt_MValue_Storagedouble* _p0)
{
    return (struct alt_MValue_Storagedouble*)new alt::MValue::Storage<double>((struct alt::MValue::Storage<double> &&)_p0);
}

CAPI void alt_MValue_Storagedouble_Assign_constMValue_StoragedoubleRef(struct alt_MValue_Storagedouble* _instance, struct alt_MValue_Storagedouble* _p0)
{
    ((alt::MValue::Storage<double> *)_instance)->operator=((const struct alt::MValue::Storage<double> &)_p0);
}

CAPI void alt_MValue_Storagedouble_Assign_MValue_StoragedoubleRefRef(struct alt_MValue_Storagedouble* _instance, struct alt_MValue_Storagedouble* _p0)
{
    ((alt::MValue::Storage<double> *)_instance)->operator=((struct alt::MValue::Storage<double> &&)_p0);
}

CAPI struct alt_MValue_Storagedouble* alt_MValue_Storagedouble_Create_2()
{
    return (struct alt_MValue_Storagedouble*)new alt::MValue::Storage<double>();
}

CAPI void alt_MValue_StorageString_free(struct alt_MValue_StorageString* ptr)
{
    delete (struct alt::MValue::Storage<class alt::String> *)ptr;
}

CAPI struct alt_MValue_StorageString* alt_MValue_StorageString_Create(struct alt_MValue_StorageString* _p0)
{
    return (struct alt_MValue_StorageString*)new alt::MValue::Storage<alt::String>((const struct alt::MValue::Storage<class alt::String> &)_p0);