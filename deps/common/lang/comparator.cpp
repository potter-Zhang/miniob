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
// Created by wangyunlai on 2021/6/11.
//

#include <string.h>
#include <algorithm>
#include "common/defs.h"
#include "date.h"

bool like_match(const char* s1, const char* s2) {
    while (*s1 != '\0' && (*s1 == *s2 || *s2 == '_')) {
        if (*s2 != '\'') {
            s1++;
        }
        s2++;
    }

    if (*s2 == '\0') {
        return *s1 == '\0';
    }

    if (*s2 == '%') {
        s2++; // skip '%'
        if (*s2 == '\0') {
            return true;
        }
        while (*s1 != '\0') {
            if (like_match(s1, s2)) {
                return true;
            }
            s1++;
        }
    }

    return false;
}

namespace common {


int compare_int(void *arg1, void *arg2)
{
  int v1 = *(int *)arg1;
  int v2 = *(int *)arg2;
return v1 - v2;
}

int compare_float(void *arg1, void *arg2)
{
  float v1 = *(float *)arg1;
  float v2 = *(float *)arg2;
  float cmp = v1 - v2;
  if (cmp > EPSILON) {
    return 1;
  }
  if (cmp < -EPSILON) {
    return -1;
  }
  return 0;
}

int compare_date(void *arg1, void *arg2)
{
  int v1 = ((Date *)arg1)->value();
  int v2 = ((Date *)arg2)->value();
  return v1 - v2;
}

int compare_string(void *arg1, int arg1_max_length, void *arg2, int arg2_max_length)
{
  const char *s1 = (const char *)arg1;
  const char *s2 = (const char *)arg2;
  int maxlen = std::min(arg1_max_length, arg2_max_length);
  int result = strncmp(s1, s2, maxlen);
  if (0 != result) {
    return result;
  }

  if (arg1_max_length > maxlen) {
    return s1[maxlen] - 0;
  }

  if (arg2_max_length > maxlen) {
    return 0 - s2[maxlen];
  }
  return 0;
}

// 认为第一个是chars，第二个是float，null的话要注意arg1和length，length应该是不计'\0'
int compare_string_float(void *arg1, int length, void * arg2) {
  char *s1 = new char[length + 1];
  s1[length] = '\0';
  memcpy(s1, arg1, length);
  float v1 = atof((const char*)s1);
  float v2 = *(float*)arg2;
  float cmp = v1 - v2;
  int result = 0;

  if (cmp > EPSILON) {
    result = 1;
  }
  else if (cmp < -EPSILON) {
    result = -1;
  }
  delete[] s1;
  return result;
}

// 认为第一个是float，第二个是chars，null的话要注意arg1和length，length应该是不计'\0'
int compare_float_string(void *arg1, void * arg2, int length){
  char *s2 = new char[length + 1];
  s2[length] = '\0';
  memcpy(s2, arg2, length);
  float v1 = *(float*)arg1;
  float v2 = atof((const char*)s2);
  float cmp = v1 - v2;
  int result = 0;
  if (cmp > EPSILON) {
    result = 1;
  }
  if (cmp < -EPSILON) {
    result = -1;
  }
  delete[] s2;
  return result;
}

// 认为第一个是chars，第二个是int，null的话要注意arg1和length，length应该是不计'\0'
int compare_string_int(void *arg1, int length, void * arg2){
  char *s1 = new char[length + 1];
  s1[length] = '\0';
  memcpy(s1, arg1, length);
  int v1 = atoi((const char*)s1);
  int v2 = *(int*)arg2;
  return v1 - v2;
}

// 认为第一个是int，第二个是chars，null的话要注意arg1和length，length应该是不计'\0'
int compare_int_string(void *arg1, void * arg2, int length){
  char *s2 = new char[length + 1];
  s2[length] = '\0';
  memcpy(s2, arg2, length);
  int v1 = *(int*)arg1;
  int v2 = atoi((const char*)s2);
  return v1 - v2;
}

int compare_like(void *arg1, void *arg2)
{
  const char *s1 = (const char*)arg1;
  const char *s2 = (const char*)arg2;
  if(like_match(s1, s2)) {
    return 1; // Match
  }
  else{
    return 0;
  }
}

int compare_bytes(void *arg1, int arg1_max_length, void *arg2, int arg2_max_length)
{
  const char *s1 = (const char *)arg1;
  const char *s2 = (const char *)arg2;
  int maxlen = std::min(arg1_max_length, arg2_max_length);

  for (int i = 0; i < maxlen; i++) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
  }
  return 0;
}

} // namespace common
