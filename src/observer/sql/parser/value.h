/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai 2023/6/27
//

#pragma once

#include <string>
#include "common/lang/date.h"
#include <vector>

/**
 * @brief 属性的类型
 * 
 */
enum AttrType
{
  UNDEFINED,
  // CHARS到DATES之间是用户可以正常使用的类型，若要天街用户可以正常使用的类型(认为NULLTYPE不正常)，最好增加到CHARS和DATES之间，否则需要修改类似ChARS<= <=DATES的地方
  CHARS,          ///< 字符串类型
  INTS,           ///< 整数类型(4字节)
  FLOATS,         ///< 浮点数类型(4字节)
  DATES,          ///< 日期类型(4字节)
  BOOLEANS,       ///< boolean类型，当前不是由parser解析出来的，是程序内部使用的
  NULLTYPE,       ///< NULL值，NULL与任何数据对比，都是false；这是一个临时类型，到了resolve阶段会被解析成对应类型
};

const char *attr_type_to_string(AttrType type);
AttrType attr_type_from_string(const char *s);

class Value;

struct value_hash_name {
  size_t operator()(const Value& p) const;
};

struct vector_value_hash_name {
  size_t operator()(const std::vector<Value>& vec) const;
};

/**
 * @brief 属性的值
 * 
 */
class Value 
{
public:
  Value() = default;

  Value(AttrType attr_type, char *data, int length = 4) : attr_type_(attr_type)
  {
    this->set_data(data, length);
  }

  bool operator == (const Value& other) const;

  explicit Value(int val);
  explicit Value(float val);
  explicit Value(Date val);
  explicit Value(bool val);
  explicit Value(const char *s, int len = 0);

  Value(const Value &other) = default;
  Value &operator=(const Value &other) = default;

  void set_type(AttrType type)
  {
    this->attr_type_ = type;
  }
  void set_length(int length)
  {
    this->length_ = length;
  }
  void set_nullable(bool nullable)
  {
    this->nullable_ = nullable;
  }
  void set_is_null(bool is_null)
  {
    this->is_null_ = is_null;
  }
  void set_is_null(int i)
  {
    this->is_null_ = i;
  }
  void set_data(char *data, int length);
  void set_data(const char *data, int length)
  {
    this->set_data(const_cast<char *>(data), length);
  }
  void set_int(int val);
  void set_float(float val);
  void set_date(Date val);
  void set_boolean(bool val);
  void set_string(const char *s, int len = 0);
  void set_value(const Value &value);

  std::string to_string() const;

  int compare(const Value &other) const;

  const char *data() const;
  void get_data();
  int length() const
  {
    return length_;
  }

  AttrType attr_type() const
  {
    return attr_type_;
  }

  bool nullable() const
  {
    return nullable_;
  }
  int is_null() const
  {
    return is_null_;
  }

public:
  /**
   * 获取对应的值
   * 如果当前的类型与期望获取的类型不符，就会执行转换操作
   */
  int get_int() const;
  float get_float() const;

  bool convert_to(AttrType);
  Date get_date() const;
  std::string get_string() const;
  bool get_boolean() const;

  // friend size_t value_hash_name::operator()(const Value& p) const;
  // friend size_t vector_value_hash_name::operator()(const std::vector<Value>& vec) const;

private:
  AttrType attr_type_ = UNDEFINED;
  int length_ = 0;

  union {
    int int_value_;
    float float_value_;
    Date date_value_;
    bool bool_value_;
  } num_value_;
  std::string str_value_;

  bool nullable_ = false;
  int is_null_ = 0;
  //std::string buffer_;
  //bool buffer_allocated_ = false;
};
