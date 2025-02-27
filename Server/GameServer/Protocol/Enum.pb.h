// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Enum.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Enum_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Enum_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Enum_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE
namespace Protocol {

enum ObjectType : int {
  NONE = 0,
  PLAYER = 1,
  MONSTER = 2,
  PROJECTILE = 3,
  ObjectType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ObjectType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ObjectType_IsValid(int value);
constexpr ObjectType ObjectType_MIN = NONE;
constexpr ObjectType ObjectType_MAX = PROJECTILE;
constexpr int ObjectType_ARRAYSIZE = ObjectType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ObjectType_descriptor();
template<typename T>
inline const std::string& ObjectType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ObjectType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ObjectType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ObjectType_descriptor(), enum_t_value);
}
inline bool ObjectType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ObjectType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ObjectType>(
    ObjectType_descriptor(), name, value);
}
enum CreatureState : int {
  IDLE = 0,
  MOVING = 1,
  ATTACK = 2,
  DEAD = 3,
  CreatureState_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  CreatureState_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool CreatureState_IsValid(int value);
constexpr CreatureState CreatureState_MIN = IDLE;
constexpr CreatureState CreatureState_MAX = DEAD;
constexpr int CreatureState_ARRAYSIZE = CreatureState_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* CreatureState_descriptor();
template<typename T>
inline const std::string& CreatureState_Name(T enum_t_value) {
  static_assert(::std::is_same<T, CreatureState>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function CreatureState_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    CreatureState_descriptor(), enum_t_value);
}
inline bool CreatureState_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, CreatureState* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<CreatureState>(
    CreatureState_descriptor(), name, value);
}
enum ItemType : int {
  ITEM_TYPE_NONE = 0,
  ITEM_TYPE_WEAPON = 1,
  ITEM_TYPE_ARMOR = 2,
  ITEM_TYPE_CONSUMABLE = 3,
  ItemType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ItemType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ItemType_IsValid(int value);
constexpr ItemType ItemType_MIN = ITEM_TYPE_NONE;
constexpr ItemType ItemType_MAX = ITEM_TYPE_CONSUMABLE;
constexpr int ItemType_ARRAYSIZE = ItemType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ItemType_descriptor();
template<typename T>
inline const std::string& ItemType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ItemType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ItemType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ItemType_descriptor(), enum_t_value);
}
inline bool ItemType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ItemType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ItemType>(
    ItemType_descriptor(), name, value);
}
enum WeaponType : int {
  WEAPON_TYPE_NONE = 0,
  WEAPON_TYPE_SWORD = 1,
  WEAPON_TYPE_BOW = 2,
  WeaponType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  WeaponType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool WeaponType_IsValid(int value);
constexpr WeaponType WeaponType_MIN = WEAPON_TYPE_NONE;
constexpr WeaponType WeaponType_MAX = WEAPON_TYPE_BOW;
constexpr int WeaponType_ARRAYSIZE = WeaponType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* WeaponType_descriptor();
template<typename T>
inline const std::string& WeaponType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, WeaponType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function WeaponType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    WeaponType_descriptor(), enum_t_value);
}
inline bool WeaponType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, WeaponType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<WeaponType>(
    WeaponType_descriptor(), name, value);
}
enum ArmorType : int {
  ARMOR_TYPE_NONE = 0,
  ARMOR_TYPE_HELMET = 1,
  ARMOR_TYPE_ARMOR = 2,
  ARMOR_TYPE_BOOTS = 3,
  ArmorType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ArmorType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ArmorType_IsValid(int value);
constexpr ArmorType ArmorType_MIN = ARMOR_TYPE_NONE;
constexpr ArmorType ArmorType_MAX = ARMOR_TYPE_BOOTS;
constexpr int ArmorType_ARRAYSIZE = ArmorType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ArmorType_descriptor();
template<typename T>
inline const std::string& ArmorType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ArmorType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ArmorType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ArmorType_descriptor(), enum_t_value);
}
inline bool ArmorType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ArmorType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ArmorType>(
    ArmorType_descriptor(), name, value);
}
enum ConsumableType : int {
  CONSUMABLE_TYPE_NONE = 0,
  CONSUMABLE_TYPE_POTION = 1,
  ConsumableType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ConsumableType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ConsumableType_IsValid(int value);
constexpr ConsumableType ConsumableType_MIN = CONSUMABLE_TYPE_NONE;
constexpr ConsumableType ConsumableType_MAX = CONSUMABLE_TYPE_POTION;
constexpr int ConsumableType_ARRAYSIZE = ConsumableType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ConsumableType_descriptor();
template<typename T>
inline const std::string& ConsumableType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ConsumableType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ConsumableType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ConsumableType_descriptor(), enum_t_value);
}
inline bool ConsumableType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ConsumableType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ConsumableType>(
    ConsumableType_descriptor(), name, value);
}
enum CurrencyType : int {
  GOLD = 0,
  CurrencyType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  CurrencyType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool CurrencyType_IsValid(int value);
constexpr CurrencyType CurrencyType_MIN = GOLD;
constexpr CurrencyType CurrencyType_MAX = GOLD;
constexpr int CurrencyType_ARRAYSIZE = CurrencyType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* CurrencyType_descriptor();
template<typename T>
inline const std::string& CurrencyType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, CurrencyType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function CurrencyType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    CurrencyType_descriptor(), enum_t_value);
}
inline bool CurrencyType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, CurrencyType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<CurrencyType>(
    CurrencyType_descriptor(), name, value);
}
enum EffectType : int {
  HEAL = 0,
  ATTACK_BOOST = 1,
  DEFENSE_BOOST = 2,
  EffectType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  EffectType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool EffectType_IsValid(int value);
constexpr EffectType EffectType_MIN = HEAL;
constexpr EffectType EffectType_MAX = DEFENSE_BOOST;
constexpr int EffectType_ARRAYSIZE = EffectType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* EffectType_descriptor();
template<typename T>
inline const std::string& EffectType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, EffectType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function EffectType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    EffectType_descriptor(), enum_t_value);
}
inline bool EffectType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, EffectType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<EffectType>(
    EffectType_descriptor(), name, value);
}
enum ChatType : int {
  CHAT_TYPE_GENERAL = 0,
  CHAT_TYPE_WHISPER = 1,
  CHAT_TYPE_SYSTEM = 2,
  ChatType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ChatType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ChatType_IsValid(int value);
constexpr ChatType ChatType_MIN = CHAT_TYPE_GENERAL;
constexpr ChatType ChatType_MAX = CHAT_TYPE_SYSTEM;
constexpr int ChatType_ARRAYSIZE = ChatType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ChatType_descriptor();
template<typename T>
inline const std::string& ChatType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ChatType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ChatType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ChatType_descriptor(), enum_t_value);
}
inline bool ChatType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ChatType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ChatType>(
    ChatType_descriptor(), name, value);
}
enum ResourceType : int {
  HP = 0,
  MP = 1,
  ResourceType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  ResourceType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool ResourceType_IsValid(int value);
constexpr ResourceType ResourceType_MIN = HP;
constexpr ResourceType ResourceType_MAX = MP;
constexpr int ResourceType_ARRAYSIZE = ResourceType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ResourceType_descriptor();
template<typename T>
inline const std::string& ResourceType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ResourceType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ResourceType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ResourceType_descriptor(), enum_t_value);
}
inline bool ResourceType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, ResourceType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ResourceType>(
    ResourceType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Protocol

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::Protocol::ObjectType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ObjectType>() {
  return ::Protocol::ObjectType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::CreatureState> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::CreatureState>() {
  return ::Protocol::CreatureState_descriptor();
}
template <> struct is_proto_enum< ::Protocol::ItemType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ItemType>() {
  return ::Protocol::ItemType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::WeaponType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::WeaponType>() {
  return ::Protocol::WeaponType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::ArmorType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ArmorType>() {
  return ::Protocol::ArmorType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::ConsumableType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ConsumableType>() {
  return ::Protocol::ConsumableType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::CurrencyType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::CurrencyType>() {
  return ::Protocol::CurrencyType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::EffectType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::EffectType>() {
  return ::Protocol::EffectType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::ChatType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ChatType>() {
  return ::Protocol::ChatType_descriptor();
}
template <> struct is_proto_enum< ::Protocol::ResourceType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Protocol::ResourceType>() {
  return ::Protocol::ResourceType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
