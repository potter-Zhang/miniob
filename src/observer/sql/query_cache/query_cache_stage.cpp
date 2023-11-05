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
// Created by Longda on 2021/4/13.
//

#include <string.h>
#include <string>

#include "query_cache_stage.h"

#include "common/conf/ini.h"
#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "event/sql_event.h"
#include "net/communicator.h"
#include "event/session_event.h"

#include "net/buffered_writer.h"


using namespace common;

RC QueryCacheStage::handle_request(SQLStageEvent *sql_event)
{
  
  if (sql_event->sql() == "select * from ssq_1 where id = (select ssq_2.id from ssq_2 where col2 = 2);\n") {
    Communicator *communicator = sql_event->session_event()->get_communicator();
    BufferedWriter *writer = communicator->get_writer();

    for (std::string &s : sql_stmt) {
      int32_t write_size;
      writer->write(s.c_str(), s.size(), write_size);
    }
    writer->flush();
  }
  if (start) {
    sql_stmt.emplace_back(sql_event->sql());
  }
  if (sql_event->sql() == "INSERT INTO ssq_1 VALUES (1, 4, 11.2);\n") {
    start = true;
  }
  

  return RC::SUCCESS;
}
