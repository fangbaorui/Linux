#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<mysql/mysql.h>

int main()
{
  //初始化句柄
  MYSQL *mysql=NULL;
  mysql=mysql_init(NULL);
  if(mysql==NULL)
  {
    printf("mysql init errer\n");
    return -1;
  }

  //连接服务器
  //mysql_real_connect(句柄，服务器地址，服务器用户名，密码，数据库名称,服务器端口，套接字文件，客户端标志)
  if(mysql_real_connect(mysql,"127.0.0.1","root","","order_sys",3306,NULL,0)==NULL)
  {
    printf("connect server mysql failed:%s\n",mysql_error(mysql));
    mysql_close(mysql);
    return -1;
  }
  
  //设置客户端字符集
  if(mysql_set_character_set(mysql,"utf8")!=0){
    //打印错误原因
    printf("set charfailed:%s\n",mysql_error(mysql));
    mysql_close(mysql);
    return -1;
  }
  
  //插入
  /*char* insert="insert tb_dish values(null,'土豆盐煎肉',1800,now());";
  int ret=mysql_query(mysql,insert);
  if(ret!=0)
  {
     printf("query failed:%s\n",mysql_error(mysql));
     return -1;
  }*/

  /*char* update="update tb_dish set price=8800 where id=3;";
  int ret=mysql_query(mysql,update); 
  if(ret!=0)                                                              
  {              
    printf("query failed:%s\n",mysql_error(mysql));           
    return -1;                                                        
  }*/
  

  /*char* delete="delete from tb_dish  where id=3;";
  int ret=mysql_query(mysql,delete); 
  if(ret!=0)                                                              
  {              
    printf("query delete failed:%s\n",mysql_error(mysql));           
    return -1;                                                        
  }*/


  
  char* select="select * from tb_dish;";
  int ret=mysql_query(mysql,select); 
  if(ret!=0)                                                              
  {              
    printf("query select failed:%s\n",mysql_error(mysql));           
    return -1;
  }
  
  //获取结果集
  MYSQL_RES *res=mysql_store_result(mysql);
  if(res==NULL)
  {
    printf("store result failed:%s\n",mysql_error(mysql));
    return -1;
  }
  
  //获取条数
  int num_row=mysql_num_rows(res);
  printf("%d\tname\tprice\tctime\n");

   //遍历结果集
  for(int i=0;i<num_row;i++)
  {
    MYSQL_ROW row=mysql_fetch_row(res);
    printf("%s\t%s\t%s\t%s\n",row[0],row[1],row[2],row[3]);
  }

  //释放结果集
  mysql_free_result(res);
  //关闭
  mysql_close(mysql);

  return 0;  
}
