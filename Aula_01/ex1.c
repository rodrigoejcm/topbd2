#include <my_global.h>
#include <mysql.h>
#include <stdio.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}


void roda_query(const char *query_input, MYSQL *con)
{
  printf("%s\n", query_input );

  if (mysql_query(con, query_input))
  {
      finish_with_error(con);
  }

  MYSQL_RES *result = mysql_store_result(con);

  if (result == NULL)
  {
      finish_with_error(con);
  }

  int num_fields = mysql_num_fields(result);

  MYSQL_ROW row;
  MYSQL_FIELD *field;

  while ((row = mysql_fetch_row(result)))
  {
      for(int i = 0; i < num_fields; i++)
      {
          if (i == 0)
          {
             while(field = mysql_fetch_field(result))
             {
                printf("%s ", field->name);
             }

             printf("\n");
          }

          printf("%s  ", row[i] ? row[i] : "NULL");
      }
  }

  printf("\n");

  mysql_free_result(result);
  mysql_close(con);

  exit(0);

}

int main(int argc, char **argv)
{
  const char * query_input;
  MYSQL *con = mysql_init(NULL);

  if (con == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "guest", NULL,
          "guest", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }


  printf ("Enter a query for cars: ");
  scanf ("%s", &query_input );

  roda_query(query_input, con);



}
