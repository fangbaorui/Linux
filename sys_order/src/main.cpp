#include"db.hpp"
#include"httplib.h"
#define WWWROOT "./wwwroot"

using namespace httplib;
order_sys::TableDish *tb_dish = NULL;
order_sys::TableOrder *tb_order =NULL;

void DishInsert(const Request &req, Response &rsp)
{
  //1.业务处理
  // 1.解析正文（json反序列化），得到菜品信息
  Json::Value dish;
  Json::Reader reader;
  bool ret = reader.parse(req.body,dish);
  if(ret == false)
  {
    rsp.status = 400;
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="dish info parse failed!";
    rsp.body = writer.write(reason);
    rsp.set_header("Content - Type","application/json");
    std::cout<< "insert dish parse error\n";
    return;
  }

  //将菜品信息插入数据库
  ret = tb_dish->Insert(dish);
  if(ret == false)
  {
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql insert failed!";
    rsp.status = 500;
    rsp.body = writer.write(reason);
    rsp.set_header("Content - Type","application/json");
    std::cout << "mysql insert dish error\n";
    return;
  }
   
  //2.设置响应信息，响应状态码httplib默认设置200
  //rsp.status = 200;
  return;
}
void DishDelete(const Request &req,Response &rsp)
{
  //1.获取要删除菜品的id/dish/id
  //matches里面存放的是正则表达式（）里面捕捉到的内容
  int dish_id = std::stoi(req.matches[1]);
  //2.删除数据库操作
  bool ret = tb_dish->Delete(dish_id);
  if(ret == false)
  {
    std::cout<<" delete mysql delete dish failed!\n";
    rsp.status = 500;
    return;
  }
  return;
}
void DishUpdate(const Request &req,Response &rsp)
{
  int dish_id = std::stoi(req.matches[1]);
  Json::Value dish;
  Json::Reader reader;
  bool ret = reader.parse(req.body,dish);
  if(ret == false)
  {
    std::cout<<"update parse dish info failed!\n";
    rsp.status = 400;
    return;
  }
  dish["id"] = dish_id;
  ret = tb_dish->Update(dish);
  if(ret == false)
  {
    std::cout<<"updata mysql update dish failed!\n";
    rsp.status = 500;
    return;
  }
  return;
}
void DishGetAll(const Request &req,Response &rsp)
{
  Json::Value dishes;
  bool ret = tb_dish->SelectAll(&dishes);
  if(ret == false)
  {
    std::cout<<"select all mysql get dish failed!\n";
    rsp.status = 500;
    return;
  }
  Json::FastWriter writer;
  rsp.body = writer.write(dishes);
  rsp.set_header("Content-Type","application/json");
  return;
}
void DishGetOne(const Request &req,Response &rsp)
{
  int dish_id = std::stoi(req.matches[1]);
  Json::Value dish;
  bool ret = tb_dish->SelectOne(dish_id,&dish);
  if(ret == false)
  {
    std::cout<<"select one mysql get dish failed!\n";
    rsp.status = 500;
    return;
  }
  Json::FastWriter writer;
  rsp.body = writer.write(dish);
  return;
}

void OrderInsert(const Request &req,Response &rsp)
{
  Json::Value order;
  Json::Reader reader;
  bool ret = reader.parse(req.body,order);
  if(ret == false)
  {
    std::cout<<" insert parse order info failed!\n";
    rsp.status = 400;
    return;
  }
  ret = tb_order->Insert(order); 
  if(ret == false)
  {
    std::cout<<" insert mysql insert order failed!\n";
    rsp.status = 500;
    return;
  }
  return;
}
void OrderDelete(const Request &req,Response &rsp)
{
  int order_id = std::stoi(req.matches[1]);
  bool ret = tb_order->Delete(order_id);
  if(ret == false)
  {
    std::cout<<" delete mysql delete order failed!\n";
    rsp.status = 500;
    return;
  }
  return;
}
void OrderUpdate(const Request &req,Response &rsp)
{
  int order_id = std::stoi(req.matches[1]);
  Json::Value order;
  Json::Reader reader;
  bool ret = reader.parse(req.body,order);
  if(ret == false)
  {
    std::cout<<"insert parse order info failed!\n";
    rsp.status = 400;
    return;
  }
  order["id"] = order_id;
  ret = tb_order->Update(order);
  if(ret == false)
  {
    std::cout<<"updata mysql update order failed!\n";
    rsp.status = 500;
    return;
  }
  return;
}
void OrderGetAll(const Request &req,Response &rsp)
{
  Json::Value orders;
  bool ret = tb_order->SelectAll(&orders);
  if(ret == false)
  {
    std::cout<<"select all mysql get order failed!\n";
    rsp.status = 500;
    return;
  }
  Json::FastWriter writer;
  rsp.body = writer.write(orders);
  return;
}
void OrderGetOne(const Request &req,Response &rsp)
{

  int order_id = std::stoi(req.matches[1]);
  Json::Value order;
  bool ret = tb_order->SelectOne(order_id,&order);
  if(ret == false)
  {
    std::cout<<"select one mysql get order failed!\n";
    rsp.status = 500;
    return;
  }
  Json::FastWriter writer;
  rsp.body = writer.write(order);
  return;
}
int main()
{
  tb_dish = new order_sys::TableDish();
  tb_order = new order_sys::TableOrder();
  Server server;
  server.set_base_dir(WWWROOT);
  server.Post("/dish",DishInsert);
  //正则表达式：（\d+）表示匹配一个数字字符一次或多次
  //正则表达式：（）表示捕捉匹配括号规则的文本
  //R"()":表示括号中的字符串去除特殊字符的特殊含义
  server.Delete(R"(/dish/(\d+))",DishDelete);
  server.Put(R"(/dish/(\d+))",DishUpdate);
  server.Get(R"(/dish)",DishGetAll);
  server.Get(R"(/dish/(\d+))",DishGetOne);
  
  server.Post("/order",OrderInsert);
  server.Delete(R"(/order/(\d+))",OrderDelete);
  server.Put(R"(/order/(\d+))",OrderUpdate);
  server.Get(R"(/order)",OrderGetAll);
  server.Get(R"(/order/(\d+))",OrderGetOne);

  server.listen("0.0.0.0",9000);
  
  return 0;
}


void dish_test()
{
  order_sys::TableDish dish;
 
  
 /* Json::Value val;
  val["id"]=4;
  val["name"]="麻婆豆腐";
  val["price"]=1900;
  //dish.Insert(val);
  //dish.Update(val);
  //dish.Delete(4);
  */

  Json::Value val;
  Json::StyledWriter writer;
  //dish.SelectAll(&val);
  dish.SelectOne(2,&val);
  std::cout << writer.write(val) <<std::endl;
  
}

void order_test()
{
  order_sys::TableOrder order;
  /*Json::Value val;
  val["id"]=3;
  //val["dishes"].append(2);
  val["dishes"].append(1);
  val["status"]=1;
  //order.Insert(val);
  //order.Update(val);
  //order.Delete(3);
  */

  Json::Value val;
  Json::StyledWriter writer;
  //order.SelectAll(&val);
  order.SelectOne(1,&val);
  std::cout<<writer.write(val)<<std::endl;
}
/*
int main()
{
  //dish_test();
  order_test();
  return 0;
}*/
