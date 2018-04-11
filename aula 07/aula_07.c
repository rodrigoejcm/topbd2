#include <my_global.h>
#include <mysql.h>
#include <time.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);
  FILE *pont_arq; // cria variável ponteiro para o arquivo
  int i = 0;

  if (con == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "guest", NULL,
          "dvds", 0, NULL, 0) == NULL)
  {
      finish_with_error(con);
  }

  if (mysql_query(con, "SELECT day(rent_date) as dia,month(rent_date) as mes, week(rent_date) as semana, year(rent_date) as ano FROM rental ORDER BY rent_date;"))
  {
      finish_with_error(con);
  }

  MYSQL_RES *resultTables = mysql_store_result(con);
  MYSQL_ROW rowTables;
  MYSQL_FIELD *fieldTables;

  while ((rowTables = mysql_fetch_row(resultTables)))
  {

    char *dia = rowTables[0];
    char *mes = rowTables[1];
    char *semana = rowTables[2];
    char *ano = rowTables[3];

    char *concat;
    concat = malloc(100); /* make space for the new string (should check the return value ...) */
    strcpy(concat, "INSERT INTO dw_tempo VALUES("); /* copy name into the new var */
    char *ic = (char) i;
    strcat(concat,ic);
    strcat(concat,",");
    strcat(concat,dia);
    strcat(concat,",");
    strcat(concat,mes);
    strcat(concat,",");
    strcat(concat,semana);
    strcat(concat,",");
    strcat(concat,ano);
    strcat(concat,");");


      if (mysql_query(con, concat ))
      {
          finish_with_error(con);
      }

      i++;
  }


  mysql_free_result(resultTables);
  mysql_close(con);
  fclose(pont_arq);
  exit(0);
}
