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
// Created by Wangyunlai.wyl on 2021/5/18.
//

#include "storage/index/index_meta.h"
#include "storage/field/field_meta.h"
#include "storage/table/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_FIELDS_NAME("fields name");
const static Json::StaticString FIELD_UNIQUE("unique");

RC IndexMeta::init(const char *name, const FieldMeta &field)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  field_ = field.name();
  this->unique = false;
  return RC::SUCCESS;
}

RC IndexMeta::init(const char *name, std::vector<const FieldMeta *> &fields, bool unique) 
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  for (const FieldMeta *&field : fields) {
    fields_.emplace_back(field->name());
  }
  this->unique = unique;
  return RC::SUCCESS;
}

RC IndexMeta::init(const char *name, std::vector<std::string> &fields, bool unique) 
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  for (const std::string &field : fields) {
    fields_.emplace_back(field);
  }

  this->unique = unique;
  
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME] = name_;
  json_value[FIELD_FIELD_NAME] = field_;
  
  for (const std::string &field : fields_) {
    json_value[FIELD_FIELDS_NAME].append(field);
  }
  json_value[FIELD_UNIQUE] = unique;

  
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  const Json::Value &fields_value = json_value[FIELD_FIELDS_NAME];
  const Json::Value &unique = json_value[FIELD_UNIQUE];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::INTERNAL;
  }

  if (!field_value.isString()) {
    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
        name_value.asCString(),
        field_value.toStyledString().c_str());
    return RC::INTERNAL;
  }
 
 
  const FieldMeta *field = table.field(field_value.asCString());
  if (field_value.asString() != "" && field != nullptr) {
    LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
    return RC::SCHEMA_FIELD_MISSING;
  }

   if (unique.empty()) {
     return index.init(name_value.asCString(), *field);
  }
  

  std::vector<std::string> fields;
  for (int i = 0; i < fields_value.size(); i++) {
    fields.push_back(fields_value[i].asString());
  }
  if (!unique.isBool()) {
    LOG_ERROR("unique is not a bool");
    return RC::INTERNAL;
  }

  if (!fields.empty()) {
    return index.init(name_value.asCString(), fields, unique.asBool());
  }

  return index.init(name_value.asCString(), *field);
}

const char *IndexMeta::name() const
{
  return name_.c_str();
}

const char *IndexMeta::field() const
{
  return field_.c_str();
}

bool IndexMeta::get_unique() const
{
  return unique;
}

int IndexMeta::field_num() const
{
  return fields_.size();
}

const char *IndexMeta::fields(int i) const
{
  if (i > fields_.size())
    return nullptr;
  return fields_[i].c_str();
}

void IndexMeta::desc(std::ostream &os) const
{
  os << "index name=" << name_ << ", field=" << field_;
}