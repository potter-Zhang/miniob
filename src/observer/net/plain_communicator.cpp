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
// Created by Wangyunlai on 2023/06/25.
//

#include <exception>

#include "net/plain_communicator.h"
#include "net/buffered_writer.h"
#include "sql/expr/tuple.h"
#include "event/session_event.h"
#include "session/session.h"
#include "common/io/io.h"
#include "common/log/log.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/parser/value.h"

PlainCommunicator::PlainCommunicator()
{
  send_message_delimiter_.assign(1, '\0');
  debug_message_prefix_.resize(2);
  debug_message_prefix_[0] = '#';
  debug_message_prefix_[1] = ' ';
}

RC PlainCommunicator::read_event(SessionEvent *&event)
{
  RC rc = RC::SUCCESS;

  event = nullptr;

  int data_len = 0;
  int read_len = 0;

  const int max_packet_size = 8192;
  std::vector<char> buf(max_packet_size);

  // 持续接收消息，直到遇到'\0'。将'\0'遇到的后续数据直接丢弃没有处理，因为目前仅支持一收一发的模式
  while (true) {
    read_len = ::read(fd_, buf.data() + data_len, max_packet_size - data_len);
    if (read_len < 0) {
      if (errno == EAGAIN) {
        continue;
      }
      break;
    }
    if (read_len == 0) {
      break;
    }

    if (read_len + data_len > max_packet_size) {
      data_len += read_len;
      break;
    }

    bool msg_end = false;
    for (int i = 0; i < read_len; i++) {
      if (buf[data_len + i] == 0) {
        data_len += i + 1;
        msg_end = true;
        break;
      }
    }

    if (msg_end) {
      break;
    }

    data_len += read_len;
  }

  if (data_len > max_packet_size) {
    LOG_WARN("The length of sql exceeds the limitation %d", max_packet_size);
    return RC::IOERR_TOO_LONG;
  }
  if (read_len == 0) {
    LOG_INFO("The peer has been closed %s", addr());
    return RC::IOERR_CLOSE;
  } else if (read_len < 0) {
    LOG_ERROR("Failed to read socket of %s, %s", addr(), strerror(errno));
    return RC::IOERR_READ;
  }

  LOG_INFO("receive command(size=%d): %s", data_len, buf.data());
  event = new SessionEvent(this);
  event->set_query(std::string(buf.data()));
  return rc;
}

RC PlainCommunicator::write_state(SessionEvent *event, bool &need_disconnect)
{
  SqlResult *sql_result = event->sql_result();
  const int buf_size = 2048;
  char *buf = new char[buf_size];
  const std::string &state_string = sql_result->state_string();
  if (state_string.empty()) {
    const char *result = RC::SUCCESS == sql_result->return_code() ? "SUCCESS" : "FAILURE";
    snprintf(buf, buf_size, "%s\n", result);
  } else {
    snprintf(buf, buf_size, "%s > %s\n", strrc(sql_result->return_code()), state_string.c_str());
  }

  RC rc = writer_->writen(buf, strlen(buf) + 1);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to send data to client. err=%s", strerror(errno));
    need_disconnect = true;
    delete[] buf;
    return RC::IOERR_WRITE;
  }

  need_disconnect = false;
  delete[] buf;

  return RC::SUCCESS;
}

RC PlainCommunicator::write_debug(SessionEvent *request, bool &need_disconnect)
{
  if (!session_->sql_debug_on()) {
    return RC::SUCCESS;
  }

  SqlDebug &sql_debug = request->sql_debug();
  const std::list<std::string> &debug_infos = sql_debug.get_debug_infos();
  for (auto &debug_info : debug_infos) {
    RC rc = writer_->writen(debug_message_prefix_.data(), debug_message_prefix_.size());
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to send data to client. err=%s", strerror(errno));
      need_disconnect = true;
      return RC::IOERR_WRITE;
    }

    rc = writer_->writen(debug_info.data(), debug_info.size());
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to send data to client. err=%s", strerror(errno));
      need_disconnect = true;
      return RC::IOERR_WRITE;
    }

    char newline = '\n';
    rc = writer_->writen(&newline, 1);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to send new line to client. err=%s", strerror(errno));
      need_disconnect = true;
      return RC::IOERR_WRITE;
    }
  }

  need_disconnect = false;
  return RC::SUCCESS;
}

RC PlainCommunicator::write_result(SessionEvent *event, bool &need_disconnect)
{
  RC rc = write_result_internal(event, need_disconnect);
  if (!need_disconnect) {
    (void)write_debug(event, need_disconnect);
  }
  writer_->flush(); // TODO handle error
  return rc;
}

RC PlainCommunicator::write_result_internal(SessionEvent *event, bool &need_disconnect)
{
  RC rc = RC::SUCCESS;
  need_disconnect = true;

  SqlResult *sql_result = event->sql_result();

  if (RC::SUCCESS != sql_result->return_code() || !sql_result->has_operator()) {
    return write_state(event, need_disconnect);
  }

  rc = sql_result->open();
  if (OB_FAIL(rc)) {
    sql_result->close();
    sql_result->set_return_code(rc);
    return write_state(event, need_disconnect);
  }

  // 看是否为聚合查询
  bool is_aggregation = false;
  // 看是否为group by
  bool is_group_by = false;
  int group_by_begin;
  ProjectPhysicalOperator* ppo;
  PhysicalOperator* oper = event->sql_result()->physical_operator().get();
  if (oper->type() ==  PhysicalOperatorType::PROJECT){
    try{
      ppo = (ProjectPhysicalOperator*)oper;    
      is_aggregation = ppo->is_aggregation();
      if ((group_by_begin = ppo->group_by_begin()) != -1)
        is_group_by = true;
    }catch(std::exception e){

    }
  }

  const TupleSchema &schema = sql_result->tuple_schema();
  const int cell_num = is_group_by ? group_by_begin : schema.cell_num();

  for (int i = 0; i < cell_num; i++) {
    const TupleCellSpec &spec = schema.cell_at(i);
    const char *alias = spec.alias();
    if (nullptr != alias || alias[0] != 0) {
      if (0 != i) {
        const char *delim = " | ";
        rc = writer_->writen(delim, strlen(delim));
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to send data to client. err=%s", strerror(errno));
          return rc;
        }
      }

      int len = strlen(alias);
      rc = writer_->writen(alias, len);
      if (OB_FAIL(rc)) {
        LOG_WARN("failed to send data to client. err=%s", strerror(errno));
        sql_result->close();
        return rc;
      }
    }
  }

  if (cell_num > 0) {
    char newline = '\n';
    rc = writer_->writen(&newline, 1);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to send data to client. err=%s", strerror(errno));
      sql_result->close();
      return rc;
    }
  }

  rc = RC::SUCCESS;
  
  //std::string query = std::toupper(event->query());
  //if query.find("max")
  Tuple *tuple = nullptr;
  if (!is_aggregation && !is_group_by){
    while (RC::SUCCESS == (rc = sql_result->next_tuple(tuple))) {
      assert(tuple != nullptr);

      int cell_num = tuple->cell_num();
      for (int i = 0; i < cell_num; i++) {
        if (i != 0) {
          const char *delim = " | ";
          rc = writer_->writen(delim, strlen(delim));
          if (OB_FAIL(rc)) {
            LOG_WARN("failed to send data to client. err=%s", strerror(errno));
            sql_result->close();
            return rc;
          }
        }

        Value value;
        rc = tuple->cell_at(i, value);
        if (rc != RC::SUCCESS) {
          sql_result->close();
          return rc;
        }

        std::string cell_str = value.to_string();
        rc = writer_->writen(cell_str.data(), cell_str.size());
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to send data to client. err=%s", strerror(errno));
          sql_result->close();
          return rc;
        }
      }

      char newline = '\n';
      rc = writer_->writen(&newline, 1);
      if (OB_FAIL(rc)) {
        LOG_WARN("failed to send data to client. err=%s", strerror(errno));
        sql_result->close();
        return rc;
      }
    }

    if (rc == RC::RECORD_EOF) {
      rc = RC::SUCCESS;
    }

    if (cell_num == 0) {
      // 除了select之外，其它的消息通常不会通过operator来返回结果，表头和行数据都是空的
      // 这里针对这种情况做特殊处理，当表头和行数据都是空的时候，就返回处理的结果
      // 可能是insert/delete等操作，不直接返回给客户端数据，这里把处理结果返回给客户端
      RC rc_close = sql_result->close();
      if (rc == RC::SUCCESS) {
        rc = rc_close;
      }
      sql_result->set_return_code(rc);
      return write_state(event, need_disconnect);
      } else {

      rc = writer_->writen(send_message_delimiter_.data(), send_message_delimiter_.size());
      if (OB_FAIL(rc)) {
        LOG_ERROR("Failed to send data back to client. ret=%s, error=%s", strrc(rc), strerror(errno));
        sql_result->close();
        return rc;
      }

      need_disconnect = false;
    }
  }
  else if(is_aggregation && !is_group_by){
    std::vector<AggregationFunc> funcs = ppo->funcs();
    int size = funcs.size();
    std::vector<Value> values(size);
    /* std::vector<bool> non_funcs(size, false);
    for(int i = 0; i < funcs.size(); i++){
      if (funcs[i] == AggregationFunc::NONE)
        non_funcs[i] = true;
    } */
    bool tuple_exist = false;
    int num = 0;
    while (RC::SUCCESS == (rc = sql_result->next_tuple(tuple))) {
      assert(tuple != nullptr);

      int cell_num = tuple->cell_num();
      for (int i = 0; i < cell_num; i++) {
        Value value;
        rc = tuple->cell_at(i, value);
        if (rc != RC::SUCCESS) {
          sql_result->close();
          return rc;
        }
        if (!tuple_exist){
          if (funcs[i] != AggregationFunc::COUNTFUN && funcs[i] != AggregationFunc::AVGFUN){
            values[i] = value;
          }
          else{
            if (!(value.nullable() && value.is_null())){
              if (funcs[i] != AggregationFunc::AVGFUN)
                values[i] = Value(1);
              else{
                values[i] = value;
                num ++;
              }
            }
            else if (funcs[i] == AggregationFunc::AVGFUN)
              // 接受null，便于最后平均值是否为null的判断
              values[i] = value;
            else {
              TupleCellSpec spec = schema.cell_at(i);
              std::string spec_str = std::string(spec.alias());
              if (spec_str.find("*") == spec_str.npos)
                values[i] = Value(0);
              else
                values[i] = Value(1);
            }
          }
        }
        else{
          switch (funcs[i]){
            case AggregationFunc::NONE: {

            } break;
            case AggregationFunc::MAXFUN: {
              if (values[i].nullable() && values[i].is_null())
                values[i] = value;
              else if (!(value.nullable() && value.is_null()) && value.compare(values[i]) > 0)
                values[i] = value;
            } break;
            case AggregationFunc::MINFUN: {
              if (values[i].nullable() && values[i].is_null())
                values[i] = value;
              else if (!(value.nullable() && value.is_null()) && value.compare(values[i]) < 0)
                values[i] = value;
            } break;
            case AggregationFunc::COUNTFUN: {
              if (!(value.nullable() && value.is_null()))
                values[i].set_int(values[i].get_int() + 1);
              else{
                TupleCellSpec spec = schema.cell_at(i);
                std::string spec_str = std::string(spec.alias());
                if (spec_str.find("*") != spec_str.npos)              
                  values[i].set_int(values[i].get_int() + 1);
              }
            } break;
            case AggregationFunc::AVGFUN: {
              if (!(value.nullable() && value.is_null())){
                num ++;
                switch (value.attr_type()) {
                  // INTS时，最后的结果放在Value.float_value_
                  case AttrType::INTS: {
                    values[i].set_int(values[i].get_int() + value.get_int());
                  } break;
                  case AttrType::FLOATS: {
                    values[i].set_float(values[i].get_float() + value.get_float());
                  } break;
                  default: {
                    LOG_WARN("invalid type to calculate average: %s", ppo->tuple().speces()[i]->alias());
                    writer_->clear();
                    sql_result->close();
                    sql_result->set_return_code(RC::INVALID_ARGUMENT);                    
                    return write_state(event, need_disconnect);
                  } break;
                }
              }
            } break;
          }
        }        
      }

      tuple_exist = true;
    }
    if (tuple_exist){
      for (int i = 0; i < size; i++){
        if (i != 0){
          const char *delim = " | ";
            rc = writer_->writen(delim, strlen(delim));
            if (OB_FAIL(rc)) {
              LOG_WARN("failed to send data to client. err=%s", strerror(errno));
              sql_result->close();
              return rc;
            }
        }
        std::string cell_str;
        // 计算平均数时，结果都用浮点数表示
        if (funcs[i] == AggregationFunc::AVGFUN){
          if (!(values[i].nullable() && values[i].is_null())){
            if (values[i].attr_type() == AttrType::INTS){
              values[i].set_float(values[i].get_int());
              values[i].set_type(AttrType::FLOATS);
            }
            values[i].set_float(values[i].get_float() / num);
          }
        }
        cell_str = values[i].to_string();
        rc = writer_->writen(cell_str.data(), cell_str.size());
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to send data to client. err=%s", strerror(errno));
          sql_result->close();
          return rc;
        }
      }

      char newline = '\n';
      rc = writer_->writen(&newline, 1);
      if (OB_FAIL(rc)) {
        LOG_WARN("failed to send data to client. err=%s", strerror(errno));
        sql_result->close();
        return rc;
      }
    }
    rc = writer_->writen(send_message_delimiter_.data(), send_message_delimiter_.size());
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to send data back to client. ret=%s, error=%s", strrc(rc), strerror(errno));
      sql_result->close();
      return rc;
    }

    need_disconnect = false;
  }
  else if(is_group_by){
    std::vector<AggregationFunc> funcs = ppo->funcs();
    int size = funcs.size();
    //std::vector<Value> values(size);
    /* std::vector<bool> non_funcs(size, false);
    for(int i = 0; i < funcs.size(); i++){
      if (funcs[i] == AggregationFunc::NONE)
        non_funcs[i] = true;
    } */

    // 存储大量group by列是否被访问过
    typedef std::unordered_map<std::vector<Value>, int, vector_value_hash_name> group_by_num;
    group_by_num num;
    typedef std::unordered_map<std::vector<Value>, std::vector<Value>, vector_value_hash_name> group_by_type;
    group_by_type group_by_map;
    bool begin = true;
    Value value;
    while (RC::SUCCESS == (rc = sql_result->next_tuple(tuple))) {
      assert(tuple != nullptr);

      // 感觉cell_num不为0
      int cell_num = tuple->cell_num();
      std::vector<Value> group_value;
      for (int i = group_by_begin; i < cell_num; i++){
        rc = tuple->cell_at(i, value);
        if (rc != RC::SUCCESS) {
          sql_result->close();
          return rc;
        }
        group_value.push_back(value);
      }
      std::pair<std::vector<Value>, std::vector<Value>> *current_group;
      std::pair<std::vector<Value>, int> *current_num;
      auto iter_group = group_by_map.find(group_value);
      auto iter_num = num.find(group_value);
      bool not_found = false;
      if (iter_group == group_by_map.end()){
        // 后面别忘了把current_group插入到group_by_map
        current_group = new std::pair<std::vector<Value>, std::vector<Value>>(
          std::pair<std::vector<Value>, std::vector<Value>>(group_value, std::vector<Value>(group_by_begin)));
        // 后面别忘了把current_num插入到num
        current_num = new std::pair<std::vector<Value>, int>(std::pair<std::vector<Value>, int>(group_value, 0));
        not_found = true;
      }
      else{
        current_group = (std::pair<std::vector<Value>, std::vector<Value>> *)&(*iter_group);
        current_num = (std::pair<std::vector<Value>, int> *)&(*iter_num);
      }
      std::vector<Value> &values = current_group->second;
      for (int i = 0; i < group_by_begin; i++) {
        Value value;
        rc = tuple->cell_at(i, value);
        if (rc != RC::SUCCESS) {
          sql_result->close();
          return rc;
        }
        if (current_num->second == 0){
          if (funcs[i] != AggregationFunc::COUNTFUN && funcs[i] != AggregationFunc::AVGFUN) {
            values[i] = value;
          }
          else{
            if (!(value.nullable() && value.is_null())){
              if (funcs[i] != AggregationFunc::AVGFUN)
                values[i] = Value(1);
              else{
                values[i] = value;
                current_num->second ++;
              }
            }
            else if (funcs[i] == AggregationFunc::AVGFUN)
              // 接受null，便于最后平均值是否为null的判断
              values[i] = value;
            else {
              TupleCellSpec spec = schema.cell_at(i);
              std::string spec_str = std::string(spec.alias());
              if (spec_str.find("*") == spec_str.npos)
                values[i] = Value(0);
              else
                values[i] = Value(1);
            }
          }
        }
        else{
          switch (funcs[i]){
            case AggregationFunc::NONE: {

            } break;
            case AggregationFunc::MAXFUN: {
              if (values[i].nullable() && values[i].is_null())
                values[i] = value;
              else if (!(value.nullable() && value.is_null()) && value.compare(values[i]) > 0)
                values[i] = value;
            } break;
            case AggregationFunc::MINFUN: {
              if (values[i].nullable() && values[i].is_null())
                values[i] = value;
              else if (!(value.nullable() && value.is_null()) && value.compare(values[i]) < 0)
                values[i] = value;
            } break;
            case AggregationFunc::COUNTFUN: {
              if (!(value.nullable() && value.is_null()))
                values[i].set_int(values[i].get_int() + 1);
              else{
                TupleCellSpec spec = schema.cell_at(i);
                std::string spec_str = std::string(spec.alias());
                if (spec_str.find("*") != spec_str.npos)              
                  values[i].set_int(values[i].get_int() + 1);
              }
            } break;
            case AggregationFunc::AVGFUN: {
              if (!(value.nullable() && value.is_null())){
                current_num->second ++;
                switch (value.attr_type()) {
                  // INTS时，最后的结果放在Value.float_value_
                  case AttrType::INTS: {
                    values[i].set_int(values[i].get_int() + value.get_int());
                  } break;
                  case AttrType::FLOATS: {
                    values[i].set_float(values[i].get_float() + value.get_float());
                  } break;
                  default: {
                    LOG_WARN("invalid type to calculate average: %s", ppo->tuple().speces()[i]->alias());
                    writer_->clear();
                    sql_result->close();
                    sql_result->set_return_code(RC::INVALID_ARGUMENT);                    
                    return write_state(event, need_disconnect);
                  } break;
                }
              }
            } break;
          }
        }        
      }
      if (not_found){
        group_by_map.insert(*current_group);
        num.insert(*current_num);
        delete current_group;
        delete current_num;
      }

      //tuple_exist = true;
    }
    //if (tuple_exist){
    for (auto iter = group_by_map.begin(); iter != group_by_map.end(); iter ++){
      std::vector<Value> values = (*iter).second;
      for (int i = 0; i < size; i++){
        if (i != 0){
          const char *delim = " | ";
            rc = writer_->writen(delim, strlen(delim));
            if (OB_FAIL(rc)) {
              LOG_WARN("failed to send data to client. err=%s", strerror(errno));
              sql_result->close();
              return rc;
            }
        }
        std::string cell_str;
        // 计算平均数时，结果都用浮点数表示
        if (funcs[i] == AggregationFunc::AVGFUN){
          if (!(values[i].nullable() && values[i].is_null())){
            if (values[i].attr_type() == AttrType::INTS){
              values[i].set_float(values[i].get_int());
              values[i].set_type(AttrType::FLOATS);
            }
            values[i].set_float(values[i].get_float() / num.find(iter->first)->second);
          }
        }
        
        cell_str = values[i].to_string();
        rc = writer_->writen(cell_str.data(), cell_str.size());
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to send data to client. err=%s", strerror(errno));
          sql_result->close();
          return rc;
        }
      }

      char newline = '\n';
      rc = writer_->writen(&newline, 1);
      if (OB_FAIL(rc)) {
        LOG_WARN("failed to send data to client. err=%s", strerror(errno));
        sql_result->close();
        return rc;
      }
    }
    rc = writer_->writen(send_message_delimiter_.data(), send_message_delimiter_.size());
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to send data back to client. ret=%s, error=%s", strrc(rc), strerror(errno));
      sql_result->close();
      return rc;
    }

    need_disconnect = false;
  }

  RC rc_close = sql_result->close();
  if (OB_SUCC(rc)) {
    rc = rc_close;
  }

  return rc;
}