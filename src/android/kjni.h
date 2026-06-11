/*
    SPDX-FileCopyrightText: 2019-2022 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KJNIEXTRAS_JNI_H
#define KJNIEXTRAS_JNI_H

#include <QJniObject>
#include <QtJniTypes>

#include <utility>

// clang-format off
///@cond internal

// determine how many elements are in __VA_ARGS__
#define KJNI_PP_NARG(...) KJNI_PP_NARG_(__VA_ARGS__ __VA_OPT__(, ) KJNI_PP_RSEQ_N())
#define KJNI_PP_NARG_(...) KJNI_PP_ARG_N(__VA_ARGS__)
#define KJNI_PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, N, ...) N
#define KJNI_PP_RSEQ_N() 7, 6, 5, 4, 3, 2, 1, 0

// preprocessor-level token concat
#define KJNI_PP_CONCAT(arg1, arg2) KJNI_PP_CONCAT1(arg1, arg2)
#define KJNI_PP_CONCAT1(arg1, arg2) KJNI_PP_CONCAT2(arg1, arg2)
#define KJNI_PP_CONCAT2(arg1, arg2) arg1##arg2

///@endcond

/** Annotates a class for holding JNI method or property wrappers.
 *
 *  @param ThisType the name of the class this is added to. Has to be derived from QtJniTypes::JObject<T>.
 *
 *  @see KJNI_DECLARE_CLASS
 */
#define KJNI_OBJECT(ThisType) \
    typedef ThisType _kjni_ThisType; \
public: \
    using JObject::JObject; \
private:

///@cond internal
#define KJNI_DECLARE_CLASS_1(Name, Type, ...) \
template <> struct ::QtJniTypes::Traits<Type> { \
    static constexpr auto className() { return QtJniTypes::CTString(Name); } \
    static constexpr auto signature() { return QtJniTypes::CTString("L") + className() + QtJniTypes::CTString(";"); } \
    static auto convertToJni(JNIEnv *, const Type &value) { return value.object(); } \
    static auto convertFromJni(QJniObject &&object) { return Type(std::move(object)); } \
};

#define KJNI_DECLARE_CLASS_2(N1, N2, ...) \
    KJNI_DECLARE_CLASS_1(N1 #N2, __VA_ARGS__)
#define KJNI_DECLARE_CLASS_3(N1, N2, ...) \
    KJNI_DECLARE_CLASS_2(N1 #N2 "/" , __VA_ARGS__)
#define KJNI_DECLARE_CLASS_4(N1, N2, ...) \
    KJNI_DECLARE_CLASS_3(N1 #N2 "/" , __VA_ARGS__)
#define KJNI_DECLARE_CLASS_5(N1, N2, ...) \
    KJNI_DECLARE_CLASS_4(N1 #N2 "/" , __VA_ARGS__)
#define KJNI_DECLARE_CLASS_6(N1, N2, ...) \
    KJNI_DECLARE_CLASS_5(N1 #N2 "/" , __VA_ARGS__)

#define KJNI_DECLARE_CLASS_(N, name, ...) KJNI_PP_CONCAT(KJNI_DECLARE_CLASS_, N)(name, __VA_ARGS__)
///@endcond

/** Create Qt JNI type traits for @p Type.
 *  Must be called after declaring a class containing @c KJNI_OBJECT.
 */
#define KJNI_DECLARE_CLASS(...) KJNI_DECLARE_CLASS_(KJNI_PP_NARG(__VA_ARGS__), "", __VA_ARGS__)


///@cond internal
#define KJNI_DECLARE_NESTED_CLASS_2(Name, NestedName, Type, ...) \
template <> struct ::QtJniTypes::Traits<Type> { \
    static constexpr auto className() { return QtJniTypes::CTString(Name "$" #NestedName); } \
    static constexpr auto signature() { return QtJniTypes::CTString("L") + className() + QtJniTypes::CTString(";"); } \
    static auto convertToJni(JNIEnv *, const Type &value) { return value.object(); } \
    static auto convertFromJni(QJniObject &&object) { return Type(std::move(object)); } \
};

#define KJNI_DECLARE_NESTED_CLASS_3(N1, N2, ...) \
    KJNI_DECLARE_NESTED_CLASS_2(N1 #N2, __VA_ARGS__)
#define KJNI_DECLARE_NESTED_CLASS_4(N1, N2, ...) \
    KJNI_DECLARE_NESTED_CLASS_3(N1 #N2 "/", __VA_ARGS__)
#define KJNI_DECLARE_NESTED_CLASS_5(N1, N2, ...) \
    KJNI_DECLARE_NESTED_CLASS_4(N1 #N2 "/", __VA_ARGS__)
#define KJNI_DECLARE_NESTED_CLASS_6(N1, N2, ...) \
    KJNI_DECLARE_NESTED_CLASS_5(N1 #N2 "/", __VA_ARGS__)
#define KJNI_DECLARE_NESTED_CLASS_7(N1, N2, ...) \
    KJNI_DECLARE_NESTED_CLASS_6(N1 #N2 "/", __VA_ARGS__)

#define KJNI_DECLARE_NESTED_CLASS_(N, name, ...) KJNI_PP_CONCAT(KJNI_DECLARE_NESTED_CLASS_, N)(name, __VA_ARGS__)
///@endcond

/** Same as KJNI_DECLARE_CLASS, but for nested Java classes. */
#define KJNI_DECLARE_NESTED_CLASS(...) KJNI_DECLARE_NESTED_CLASS_(KJNI_PP_NARG(__VA_ARGS__), "", __VA_ARGS__)

///@cond internal

// method parameter generation
#define KJNI_PARAM(Type, Name) std::conditional_t<QtJniTypes::isPrimitiveType<Type>(), Type, const Type&> Name

#define KJNI_PARAMS_0(accu, arg)
#define KJNI_PARAMS_1(accu, arg, ...) KJNI_PARAM(arg, a1)
#define KJNI_PARAMS_2(accu, arg, ...) KJNI_PARAM(arg, a2), KJNI_PARAMS_1(accu, __VA_ARGS__)
#define KJNI_PARAMS_3(accu, arg, ...) KJNI_PARAM(arg, a3), KJNI_PARAMS_2(accu, __VA_ARGS__)

#define KJNI_PARAMS_(N, accu, ...) KJNI_PP_CONCAT(KJNI_PARAMS_, N)(accu, __VA_ARGS__)
#define KJNI_PARAMS(...) KJNI_PARAMS_(KJNI_PP_NARG(__VA_ARGS__), "", __VA_ARGS__)

// method argument forwarding generation
#define KJNI_ARG(Type, Name) Name
#define KJNI_ARGS_0(accu, arg)
#define KJNI_ARGS_1(accu, arg, ...) KJNI_ARG(arg, a1)
#define KJNI_ARGS_2(accu, arg, ...) KJNI_ARG(arg, a2), KJNI_ARGS_1(accu, __VA_ARGS__)
#define KJNI_ARGS_3(accu, arg, ...) KJNI_ARG(arg, a3), KJNI_ARGS_2(accu, __VA_ARGS__)

#define KJNI_ARGS_(N, accu, ...) KJNI_PP_CONCAT(KJNI_ARGS_, N)(accu, __VA_ARGS__)
#define KJNI_ARGS(...) KJNI_ARGS_(KJNI_PP_NARG(__VA_ARGS__), "", __VA_ARGS__)

///@endcond

/**
 * Wrap a JNI method call.
 * This will add a method named @p Name to the current class. Argument types are checked at compile time.
 *
 * @param RetT The return type. Must either be a primitive type or a type declared with @c Q_DECLARE_JNI_CLASS.
 * @param Name The name of the method. Must match the JNI method to be called exactly.
 * @param Args A list or argument types (can be empty). Must either be primitive types or types declared
 *        with @c Q_DECLARE_JNI_CLASS
 */
#define KJNI_METHOD(RetT, Name, ...) \
    inline auto Name(KJNI_PARAMS(__VA_ARGS__)) const \
    { \
        return callMethod<RetT>("" #Name __VA_OPT__(, ) KJNI_ARGS(__VA_ARGS__)); \
    }

/**
 * Wrap a JNI static method call.
 * This will add a static method named @p Name to the current class. Argument types are checked at compile time.
 *
 * This macro can only be placed in classes deriving from QtJniTypes::JObject<T>.
 *
 * @param RetT The return type. Must either be a primitive type or a type declared with @c JNI_TYPE
 * @param Name The name of the method. Must match the JNI method to be called exactly.
 * @param Args A list or argument types (can be empty). Must either be primitive types or types declared
 *        with @c JNI_TYPE.
 */
#define JNI_STATIC_METHOD(RetT, Name, ...) \
    static inline auto Name(KJNI_PARAMS(__VA_ARGS__)) \
    { \
        return callStaticMethod<RetT>("" #Name __VA_OPT__(, ) KJNI_ARGS(__VA_ARGS__)); \
    }

/**
 * Wrap a JNI constructor call.
 * This will add a constructor named @p Name to the current class. Argument types are checked at compile time.
 *
 * This macro can only be placed in classes deriving from QtJniTypes::JObject<T>.
 *
 * @param Name The name of the method. Must match the JNI method to be called exactly.
 * @param Args A list or argument types (can be empty). Must either be primitive types or types declared
 *        with @c Q_DECLARE_JNI_CLASS.
 */
#define KJNI_CONSTRUCTOR(Name, ...) \
    Name(KJNI_PARAMS(__VA_ARGS__)) : QtJniTypes::JObject<Class>(KJNI_ARGS(__VA_ARGS__)) {}

///@cond internal
namespace KJniExtras::Detail
{

/** Wrapper for static properties. */
template<typename PropType, typename ClassType, typename NameHolder>
struct StaticProperty {
    operator auto() const
    {
        return ClassType::template getStaticField<PropType>(NameHolder::jniName());
    }
};

/** Wrapper for non-static properties. */
template<typename PropType, typename ClassType, typename NameHolder, typename OffsetHolder>
class Property
{
private:
    ClassType* getThis()
    {
        return reinterpret_cast<ClassType *>(reinterpret_cast<char *>(this) - OffsetHolder::offset());
    }
    const ClassType* getThis() const
    {
        return reinterpret_cast<const ClassType *>(reinterpret_cast<const char *>(this) - OffsetHolder::offset());
    }
public:
    operator PropType() const
    {
        return getThis()->template getField<PropType>(NameHolder::jniName());
    }
    Property &operator=(std::conditional_t<QtJniTypes::isPrimitiveType<PropType>(), PropType, const PropType&> value)
    {
        getThis()->setField(NameHolder::jniName(), value);
        return *this;
    }
};

}
///@endcond

/**
 * Wrap a static final property.
 * This will add a public static member named @p name to the current class. This member defines an
 * implicit conversion operator which will trigger the corresponding a JNI read operation.
 * Can only be placed in classes with a @c KJNI_OBJECT.
 *
 * @note Make sure to access this member with a specific type, assigning to an @c auto variable will
 * copy the wrapper type, not read the property value.
 *
 * @param type The data type of the property.
 * @param name The name of the property.
 */
#define KJNI_CONSTANT(type, name) \
private: \
    struct _kjni_##name##__NameHolder { \
        static constexpr const char *jniName() { return "" #name; } \
    }; \
public: \
    static inline const KJniExtras::Detail::StaticProperty<type, _kjni_ThisType, _kjni_##name##__NameHolder> name;

/**
 * Wrap a member property.
 * This will add a public zero-size member named @p name to the current class. This member defines an
 * implicit conversion operator which will trigger the corresponding a JNI read operation, as well
 * as an overloaded assignment operator for the corresponding write operation.
 * Can only be placed in classes with a @c KJNI_OBJECT.
 *
 * @note Make sure to access this member with a specific type, assigning to an @c auto variable will
 * copy the wrapper type, not read the property value.
 *
 * @param type The data type of the property.
 * @param name The name of the property.
 */
#define KJNI_PROPERTY(type, name) \
private: \
    struct _kjni_##name##__NameHolder { \
        static constexpr const char *jniName() { return "" #name; } \
    }; \
    struct _kjni_##name##__OffsetHolder { \
        static constexpr std::size_t offset() \
        { \
            QT_WARNING_PUSH QT_WARNING_DISABLE_INVALID_OFFSETOF return offsetof(_kjni_ThisType, name); \
            QT_WARNING_POP \
        } \
    }; \
public: \
    [[no_unique_address]] KJniExtras::Detail::Property<type, _kjni_ThisType, _kjni_##name##__NameHolder, _kjni_##name##__OffsetHolder> name;

// clang-format on

#endif
