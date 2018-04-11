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
  if (!YAP_Unify(arg_conn, YAP_MkIntTerm((long) conn)))
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
    MYSQL_RES *res_set;
    MYSQL *conn = (MYSQL *) YAP_IntOfTerm(YAP_ARG1);
    char *query = YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2));
    mysql_query(conn, query);
    res_set = mysql_store_result(conn);
    return(YAP_Unify(YAP_ARG3, YAP_MkIntTerm((long) res_set)));
}



// db_row(X,[A,B]).
int c_db_row(void) {
    MYSQL_ROW row;
    int i;
    MYSQL_RES *res_set = (MYSQL_RES *) YAP_IntOfTerm(YAP_ARG1);
    int arity = mysql_num_fields(res_set);
    if ((row = mysql_fetch_row(res_set)) != NULL) {
      YAP_Term head, list = YAP_ARG2;
      for (i = 0; i < arity; i++) {
        head = YAP_HeadOfTerm(list);
        list = YAP_TailOfTerm(list);
        if (!YAP_Unify(head, YAP_MkIntTerm((long) atoi(row[i])))) return FALSE;
      }
      return TRUE;
    }
    mysql_free_result(res_set);
    YAP_cut_fail();
}


int c_db_assert(void) {
    MYSQL_ROW row;
    MYSQL_FIELD *field;
    char *query;
    int arity;
    int i;
    YAP_Functor f_pred, f_assert;
    YAP_Term t_pred, *t_args, t_assert;
    MYSQL *conn = (MYSQL *) YAP_IntOfTerm(YAP_ARG1);
    sprintf(query,"SELECT * FROM %s",YAP_AtomName(YAP_AtomOfTerm(YAP_ARG2)));
    mysql_query(conn, query);
    MYSQL_RES *res_set = mysql_store_result(conn);
    arity = mysql_num_fields(res_set); // get the number of column fields
    f_pred = YAP_MkFunctor(YAP_AtomOfTerm(YAP_ARG3), arity);
    f_assert = YAP_MkFunctor(YAP_LookupAtom("assert"), 1);
    while ((row = mysql_fetch_row(res_set)) != NULL) {
        for (i = 0; i < arity; i++) {
            t_args[i] = YAP_MkIntTerm((long) row[i]);
        }
        t_pred = YAP_MkApplTerm(f_pred, arity, t_args);
        t_assert = YAP_MkApplTerm(f_assert, 1, &t_pred);
        YAP_CallProlog(t_assert); // assert the row as a Prolog fact
    }
    mysql_free_result(res_set);
return TRUE;}


void init_connection(void)
{
  // um para cada predicado (funcao=
 YAP_UserCPredicate("db_connect", c_db_connect, 5); // argumento final define o numero de variavesi
 YAP_UserCPredicate("db_disconnect", c_db_disconnect, 1);
 YAP_UserCPredicate("db_query", c_db_query, 3);
 YAP_UserCPredicate("db_assert", c_db_assert, 3);
 //YAP_UserCPredicate("db_row", c_db_row, 2);
 YAP_UserBackCutCPredicate("db_row", c_db_row, c_db_row, NULL, 2, 0);
 // a primeira funçao é a primeira a ser chamada. a segunda é para continuar chamando, o 2 é a aridade(No parametros) e o 0 é que nao é necessario manter nada em memoria
 //YAP_UserBackCutCPredicate("query_row", c_db_query, c_db_row, NULL, 3, 2);
 //YAP_UserBackCutCPredicate("n100", start_n100, continue_n100, NULL, 1, 1);
 //YAP_UserCPredicate("db_assert", c_db_assert, 3);
}



 /*
COMPILAÇÃO
gcc -c -shared -fPIC aula3_dbconn.c `mysql_config --cflags --libs
LINKAGEM
ld -shared -o aula3_dbconn.so aula3_dbconn.o


YAP
load_foreign_files(['aula3_dbconn'],[],init_connection).   -> carrega arquivo SO
load_foreign_files(['aula4_dbquery'],[],init_connection).
db_connect(localhost,guest,'',dvds,CONN).  -> esses saos os YAP_ARG1/2/3.... strings começadas em minusculo nao precisam de aspas sim'ples. o vazio precisa


load_foreign_files(['aula4_dbquery'],[],init_connection).
db_connect(localhost,guest,'',dvds,CONN), db_query(CONN,'SELECT * FROM film_actor',X).



db_connect(localhost,guest,'',dvds,CONN),
db_assert(CONN,'film_actor',4)



Em CONN é guardado um ponteiro de memória com a conexao
CONN = 1041571208

para desconectra é necessário passar emdereco de memoiria pois a variavel CONN é insisivel para o db_disconnect
db_disconnect(1041571208).
94361832291080

?-
film_actor(A,B):-
  db_query(CONN,'SELECT * FROM film_actor;',X),
  db_row(X,[A,B]).


   film_actor(A,B):-
    query_row(CONN,'SELECT * FROM film_actor;',X,X,[A,B]).

*/


 /*
OPERADOR =.. unifica variaveis em uma lista
OPERADOR :- parecido com uma chave em c

*/



////////////// AULA 5


// /db_row()
