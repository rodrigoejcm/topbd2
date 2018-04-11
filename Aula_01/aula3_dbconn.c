#include "my_global.h"
#include "mysql.h"
#include "Yap/YapInterface.h"
#include "unistd.h"
#include "sys/types.h"



int c_db_connect(void) {
  YAP_Term arg_host = YAP_ARG1;
  YAP_Term arg_user = YAP_ARG2;
  YAP_Term arg_passwd = YAP_ARG3;
  YAP_Term arg_database = YAP_ARG4;
  YAP_Term arg_conn = YAP_ARG5;
  MYSQL *conn;
  char *host = YAP_AtomName(YAP_AtomOfTerm(arg_host));
  char *user = YAP_AtomName(YAP_AtomOfTerm(arg_user));
  char *passwd = YAP_AtomName(YAP_AtomOfTerm(arg_passwd));
  char *database = YAP_AtomName(YAP_AtomOfTerm(arg_database));
  conn = mysql_init(NULL);
  if (conn == NULL) {
    printf("erro no init\n");
    return FALSE;
  }
  if (mysql_real_connect(conn, host, user, passwd, database, 0, NULL, 0) == NULL) {
    printf("erro no connect\n");
    return FALSE;
  }
  if (!YAP_Unify(arg_conn, YAP_MkIntTerm((int) conn)))
    return FALSE;
    return TRUE;
}

int c_db_disconnect(void) {
      YAP_Term arg_conn = YAP_ARG1;
      MYSQL *conn = (MYSQL *) YAP_IntOfTerm(arg_conn);
      mysql_close(conn);
      return TRUE;
}

int c_db_query(void) {
    MYSQL *conn = (MYSQL *) YAP_IntOfTerm(YAP_ARG1);
    char *query = YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2));
    mysql_query(conn, query);
    res_set = mysql_store_result(conn);
    return(YAP_Unify(YAP_ARG3, YAP_MkIntTerm((int) res_set)));
}


void init_connection(void)
{
  // um para cada predicado (funcao=
 YAP_UserCPredicate("db_connect", c_db_connect, 5); // argumento final define o numero de variavesi
 YAP_UserCPredicate("db_disconnect", c_db_disconnect, 1);
 YAP_UserCPredicate("db_query", c_db_query, 3);
}



 /*
COMPILAÇÃO
gcc -c -shared -fPIC aula3_dbconn.c `mysql_config --cflags --libs
LINKAGEM
ld -shared -o aula3_dbconn.so aula3_dbconn.o


YAP
load_foreign_files(['aula3_dbconn'],[],init_connection).   -> carrega arquivo SO
db_connect(localhost,guest,'',dvds,CONN).  -> esses saos os YAP_ARG1/2/3.... strings começadas em minusculo nao precisam de aspas sim'ples. o vazio precisa

Em CONN é guardado um ponteiro de memória com a conexao
CONN = 1041571208

para desconectra é necessário passar emdereco de memoiria pois a variavel CONN é insisivel para o db_disconnect
db_disconnect(1041571208).



*/
