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
// Created by WangYunlai on 2023/06/28.
//

#include <sstream>
#include <limits>
#include "sql/parser/value.h"
#include "storage/field/field.h"
#include "common/log/log.h"
#include "common/lang/comparator.h"
#include "common/lang/string.h"
#include "value.h"
#include <bits/stdc++.h>

const char *ATTR_TYPE_NAME[] = {"undefined", "chars", "text", "ints", "floats", "dates", "compound","booleans", "null"};

const char *attr_type_to_string(AttrType type)
{
  if (type >= UNDEFINED && type <= COMPOUND) {
    return ATTR_TYPE_NAME[type];
  }
  return "unknown";
}
AttrType attr_type_from_string(const char *s)
{
  for (unsigned int i = 0; i < sizeof(ATTR_TYPE_NAME) / sizeof(ATTR_TYPE_NAME[0]); i++) {
    if (0 == strcmp(ATTR_TYPE_NAME[i], s)) {
      return (AttrType)i;
    }
  }
  return UNDEFINED;
}

size_t value_hash_name::operator()(const Value& p) const
{  
  /* AttrType type = p.attr_type();
  int length = p.length();
  char * buffer = new char[length + 2];
  memcpy(buffer, &p, length);
  // 最后两位放attr type
  memcpy(buffer + length, &type, 2);
  size_t temp = *(size_t*)buffer;
  free(buffer);
  return temp; */
  int length = p.length();
  char* buffer = new char[length];
  memcpy(buffer, p.data(), length);
  std::string ss(buffer, length);
  return std::hash<std::string>{}(ss);
}

size_t vector_value_hash_name::operator()(const std::vector<Value> &vec) const {
  size_t seed = vec.size();
  for (const Value& value : vec) {
    value_hash_name vhn;
    seed ^= vhn(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  return seed;
}

size_t vector2_value_hash_name::operator()(const std::vector<std::vector<Value>> &vec) const {
  size_t seed = vec.size();
  for (const std::vector<Value>& values : vec) {
    vector_value_hash_name vhn;
    seed ^= vhn(values) + 0x9e3679b9 + (seed << 7) + (seed >> 1);
  }
  return seed;
}

size_t string_hash_name::operator()(const std::string &str) const {
  return std::hash<std::string>{}(str);
}

Value::Value(int val)
{
  set_int(val);
}

Value::Value(float val)
{
  set_float(val);
}

Value::Value(Date val)
{
  set_date(val);
}

Value::Value(bool val)
{
  set_boolean(val);
}

Value::Value(const char *s, int len /*= 0*/)
{
  set_string(s, len);
}

Value::Value(date val)
{
  set_date(val.d);
}

void Value::set_data(char *data, int length)
{
  switch (attr_type_) {
    case CHARS: {
      set_string(data, length);
      return;
    } break;
    case TEXT: {
      set_text(data, length);
      return;
    } break;
    case INTS: {
      num_value_.int_value_ = *(int *)data;
      length_ = length;
      if (nullable_)
        length_ ++;
    } break;
    case FLOATS: {
      num_value_.float_value_ = *(float *)data;
      length_ = length;
      if (nullable_)
        length_ ++;
    } break;
    case DATES: {
      num_value_.date_value_ = *((Date *)data);
      length_ = length;
      if (nullable_)
        length_ ++;
    } break;
    case BOOLEANS: {
      num_value_.bool_value_ = *(int *)data != 0;
      length_ = length;
      if (nullable_)
        length_ ++;
    } break;
    default: {
      LOG_WARN("unknown data type: %d", attr_type_);
    } break;
  }
  get_data();
}
void Value::set_int(int val)
{
  attr_type_ = INTS;
  num_value_.int_value_ = val;
  length_ = sizeof(val);
  if (nullable_)
    length_ ++;
  get_data();
}

void Value::set_float(float val)
{
  attr_type_ = FLOATS;
  num_value_.float_value_ = val;
  length_ = sizeof(val);
  if (nullable_)
    length_ ++;
  get_data();
}
void Value::set_date(Date val)
{
  attr_type_ = DATES;
  num_value_.date_value_ = val;
  length_ = sizeof(val);
  if (nullable_)
    length_ ++;
  get_data();
}
void Value::set_boolean(bool val)
{
  attr_type_ = BOOLEANS;
  num_value_.bool_value_ = val;
  length_ = sizeof(val);
  if (nullable_)
    length_ ++;
  get_data();
}
void Value::set_string(const char *s, int len /*= 0*/)
{
  attr_type_ = CHARS;
  if (len > 0) {
    len = strnlen(s, len);
    str_value_.assign(s, len);
  } else {
    str_value_.assign(s);
  }
  length_ = str_value_.length();
  if (nullable_)
    length_ ++;
  get_data();
}
void Value::set_text(const char *s, int len /*= 0*/)
{
  attr_type_ = TEXT;
  if (len > 0) {
    len = strnlen(s, len);
    str_value_.assign(s, len);
  } else {
    str_value_.assign(s);
  }
  length_ = str_value_.length();
  if (nullable_)
    length_ ++;
  get_data();
}
void Value::set_date(int val)
{
  attr_type_ = DATES;
  num_value_.date_value_ = val;
  length_ = sizeof(num_value_.date_value_);
}

void Value::set_value(const Value &value)
{
  switch (value.attr_type_) {
    case INTS: {
      set_int(value.get_int());
    } break;
    case FLOATS: {
      set_float(value.get_float());
    } break;
    case DATES: {
      set_date(value.get_date());
    } break;
    case CHARS: {
      set_string(value.get_string().c_str());
    } break;
    case TEXT: {
      set_text(value.get_string().c_str());
    } break;
    case BOOLEANS: {
      set_boolean(value.get_boolean());
    } break;
    case UNDEFINED: {
      ASSERT(false, "got an invalid value type");
    } break;
  }
}

const char* Value::data() const{
  return str_value_.c_str();
}

void Value::get_data()
{
  switch (attr_type_) {
    case CHARS: {
      if (nullable_) {
        std::string temp;
        temp = is_null_;
        str_value_ = temp + str_value_;
      }
    } break;
    default: {
      if (nullable_) {
        std::string temp;
        temp = is_null_;
        str_value_ = temp + std::string((const char *)&num_value_, length_ - 1);
      }
      else {
        str_value_ = std::string((const char *)&num_value_, length_);
      }
    } break;
  }
}

std::string Value::to_string() const
{
  std::stringstream os;
  if (nullable_ && is_null_){
    os << std::string("NULL");
  }
  else{
    switch (attr_type_) {
      case INTS: {
        os << num_value_.int_value_;
      } break;
      case FLOATS: {
        os << common::double_to_str(num_value_.float_value_);
      } break;
      case DATES: {
        os << common::date_to_str(num_value_.date_value_);
      } break;
      case BOOLEANS: {
        os << num_value_.bool_value_;
      } break;
      case CHARS: {
        if (nullable_) {
          os << str_value_.substr(1);
        }
        else
          os << str_value_;
      } break;
      case NULLTYPE: {
        os << std::string("NULL");
      }
      default: {
        LOG_WARN("unsupported attr type: %d", attr_type_);
      } break;
    }
  }
  return os.str();
}

int Value::compare(const Value &other) const
{
  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
      case INTS: {
        return common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_);
      } break;
      case FLOATS: {
        return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other.num_value_.float_value_);
      } break;
      case DATES: {
        return common::compare_date((void *)&this->num_value_.date_value_, (void *)&other.num_value_.date_value_);
      } break;
      case CHARS: {
        void * pointer_this;
        if (this->nullable_)
          pointer_this = (void *)this->str_value_.c_str() + 1;
        else
          pointer_this = (void *)this->str_value_.c_str();
        void * pointer_other;
        if (other.nullable())
          pointer_other = (void *)other.str_value_.c_str() + 1;
        else
          pointer_other = (void *)other.str_value_.c_str();
        return common::compare_string(pointer_this,
            this->str_value_.length(),
            pointer_other,
            other.str_value_.length());
      } break;
      case BOOLEANS: {
        return common::compare_int((void *)&this->num_value_.bool_value_, (void *)&other.num_value_.bool_value_);
      } break;
      default: {
        LOG_WARN("unsupported type: %d", this->attr_type_);
      }
    }
  } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = this->num_value_.int_value_;
    return common::compare_float((void *)&this_data, (void *)&other.num_value_.float_value_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = other.num_value_.int_value_;
    return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other_data);
  } else if (this->attr_type_ == INTS && other.attr_type_ == CHARS) {
    return common::compare_int_string((void *)&this->num_value_.int_value_, (void *)other.str_value_.c_str(), other.str_value_.length());
  } else if (this->attr_type_ == CHARS && other.attr_type_ == INTS) {
    return common::compare_string_int((void *)this->str_value_.c_str(), this->length_, (void *)&other.num_value_.int_value_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == CHARS) {
    return common::compare_float_string((void *)&this->num_value_.float_value_, (void *)other.str_value_.c_str(), other.str_value_.length());
  } else if (this->attr_type_ == CHARS && other.attr_type_ == FLOATS) {
    return common::compare_string_float((void *)this->str_value_.c_str(), this->length_, (void *)&other.num_value_.float_value_);
  }
  LOG_WARN("not supported");
  return -1;  // TODO return rc?
}

bool Value::operator == (const Value& other) const{
  if (attr_type_ == other.attr_type_) {
    bool left_null = nullable_ && is_null_;
    bool right_null = other.nullable_ && other.is_null_;
    if (left_null && right_null)
      return true;
    if (left_null || right_null)
      return false;
    return compare(other) == 0;
  }    
  else
    return false;
  //return this->attr_type_ == other.attr_type_ && compare(other) == 0;
}

bool Value::operator < (const Value& other) const{
  if (asc_) {
    if (nullable_ && is_null_)
      return true;
    else if (other.nullable_ && other.is_null_)
      return false;
    return compare(other) < 0;
  }
  else {
    if (nullable_ && is_null_)
      return false;
    else if (other.nullable_ && other.is_null_)
      return true;
    return compare(other) > 0;
  }
}

int Value::get_int() const
{
  switch (attr_type_) {
    case CHARS: {
      try {
        return (int)(std::stol(str_value_));
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to number. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0;
      }
    }
    case INTS: {
      return num_value_.int_value_;
    }
    case FLOATS: {
      return (int)(num_value_.float_value_);
    }
    case DATES: {
      LOG_WARN("false data type. type=%d", attr_type_);
      return 0;
    } break;
    case BOOLEANS: {
      return (int)(num_value_.bool_value_);
    }
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

float Value::get_float() const
{
  switch (attr_type_) {
    case CHARS: {
      try {
        return std::stof(str_value_);
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0.0;
      }
    } break;
    case INTS: {
      return float(num_value_.int_value_);
    } break;
    case FLOATS: {
      return num_value_.float_value_;
    } break;
    case DATES: {
      LOG_WARN("false data type. type=%d", attr_type_);
      return 0;
    } break;
    case BOOLEANS: {
      return float(num_value_.bool_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

//先草率实现一下
Date Value::get_date() const
{
  switch (attr_type_) {
    case DATES: {
      return num_value_.date_value_;
    } break;
    default: {
      LOG_WARN("false data type. type=%d", attr_type_);
      return (Date)0;
    }
  }
  return (Date)0;
}

std::string Value::get_string() const
{
  return this->to_string();
}

bool Value::get_boolean() const
{
  switch (attr_type_) {
    case CHARS: {
      try {
        float val = std::stof(str_value_);
        if (val >= EPSILON || val <= -EPSILON) {
          return true;
        }

        int int_val = std::stol(str_value_);
        if (int_val != 0) {
          return true;
        }

        return !str_value_.empty();
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float or integer. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return !str_value_.empty();
      }
    } break;
    case INTS: {
      return num_value_.int_value_ != 0;
    } break;
    case FLOATS: {
      float val = num_value_.float_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case DATES: {
      LOG_WARN("false data type. type=%d", attr_type_);
      return 0;
    } break;
    case BOOLEANS: {
      return num_value_.bool_value_;
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return false;
    }
  }
  return false;
}


bool Value::convert_to(AttrType new_type) {
  if (attr_type_ == new_type)
    return true;
  if (nullable_ && is_null_){
    attr_type_ = new_type;
    length_ = 2; //图方便，可能有问题
    get_data();
    return true;
  }
  else{
    switch (new_type) {
      case CHARS: {
        set_string(to_string().c_str());
        return true;
      } break;
      case DATES: {
        if (attr_type_ != CHARS)
          return false;
        int d = get_date().value();
        if (d != 0) {
          std::string().swap(str_value_);
          set_date(d);
          return true;
        }
        return false;
      }

      case FLOATS: {
        if (attr_type_ == INTS) {
          set_float(get_int());
          return true;
        }
        return false;
      }

      case INTS: {
        if (attr_type_ == FLOATS) {
          set_int(round(get_float()));
          return true;
        }
        return false;
      }
        
      default:
        return false;
    }
  }
  return false;
}
